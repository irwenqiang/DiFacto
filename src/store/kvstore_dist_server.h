/*!
 * Copyright (c) 2015 by Contributors
 * \file mxnet_node.h
 * \brief implement mxnet nodes
 */
#ifndef DIFACTO_STORE_KVSTORE_DIST_SERVER_H_
#define DIFACTO_STORE_KVSTORE_DIST_SERVER_H_
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <future>
#include <vector>
#include "ps/ps.h"
#include "difacto/updater.h"
#include "difacto/store.h"
#include "common/kv_union.h"
#include "dmlc/timer.h"
namespace difacto {

static const int kStopServer = -1;
static const int kSyncMode = -2;

class KVStoreDistServer {
 public:
  KVStoreDistServer() {
    using namespace std::placeholders;
    ps_server_ = new ps::KVServer<float>(0);
    static_cast<ps::SimpleApp*>(ps_server_)->set_request_handle(
        std::bind(&KVStoreDistServer::CommandHandle, this, _1, _2));
    ps_server_->set_request_handle(
        std::bind(&KVStoreDistServer::DataHandle, this, _1, _2, _3));
    sync_mode_ = false;
  }

  ~KVStoreDistServer() {
    delete ps_server_;
  }

  /*  does controller is really needed?
  void set_controller(const KVStore::Controller& controller) {
    CHECK(controller);
    controller_ = controller;
  }*/

  void SetUpdater(const std::shared_ptr<Updater>& updater)  {
    CHECK(updater);
    updater_ = updater;
  }

 private:
  void CommandHandle(const ps::SimpleData& recved, ps::SimpleApp* app) {
    if (recved.head == kSyncMode) {
      sync_mode_ = true;
    } 
    app->Response(recved);
  }

  void DataHandle(const ps::KVMeta& req_meta,
                  const ps::KVPairs<real_t>& req_data,
                  ps::KVServer<real_t>* server) {
    // do some check
    CHECK_GT(req_data.keys.size(), (size_t)0) << "req_data must has keys";
    int val_type = req_meta.cmd;
    if (req_meta.push) {
      CHECK_GT(req_data.vals.size(), (size_t)0) << "pushed req_data must has vals";
    
      if (sync_mode_ && 3 == val_type) {
        // synced push
        if (merge_buf_.request.size() == 0) {
          merge_buf_.data.keys.CopyFrom(req_data.keys);
          merge_buf_.data.vals.CopyFrom(req_data.vals);
        } else {
          KVUnion(req_data.keys, req_data.vals, &merge_buf_.data.keys, &merge_buf_.data.vals);
        }
        merge_buf_.request.push_back(req_meta);

        LOG(INFO) <<"requestnum " << merge_buf_.request.size() <<" " <<val_type <<" "<<req_meta.timestamp;
        if (merge_buf_.request.size() == (size_t)ps::NumWorkers()) { // problem
          // execute updater
          CHECK(updater_);
          updater_->Update(merge_buf_.data.keys, val_type, merge_buf_.data.vals, merge_buf_.data.lens);
          for (const auto& req : merge_buf_.request) {
            server->Response(req);
          }
          merge_buf_.request.clear();
        } else {
         // merged.array.WaitToRead();
        }
      } else {
        // async push 
        CHECK(updater_);
        updater_->Update(req_data.keys, val_type, req_data.vals, req_data.lens);
        server->Response(req_meta);
        updater_->Report();
      }
    } else {
      ps::KVPairs<real_t> response;
      // pull data from updater
      CHECK(updater_);
      updater_->Get(req_data.keys, val_type, &(response.vals), &(response.lens));
      response.keys = req_data.keys;
      server->Response(req_meta, response);
    }
  }

  /**
   * \brief user defined
   */
  bool sync_mode_;
  //KVStore::Controller controller_;
  std::shared_ptr<Updater> updater_;

  struct MergeBuf {
    std::vector<ps::KVMeta> request;
    ps::KVPairs<real_t> data;
  } merge_buf_;

  ps::KVServer<float>* ps_server_;
};

}  // namespace difacto

#endif  // DIFACTO_STORE_KVSTORE_DIST_SERVER_H_
