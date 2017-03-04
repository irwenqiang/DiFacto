#export PS_VERBOSE=2

#dmlc-core/tracker/dmlc-submit --cluster=yarn --num-workers=5 --num-servers=2  build/difacto task=train data_in=hdfs:///user/vinceywang/data/agaricus.txt.train model_out=hdfs:///user/vinceywang/model/difacto learner=sgd max_num_epochs=3 batch_size=100 V_dim=0 l1=0 l2=0 has_aux=1

#dmlc-core/tracker/dmlc-submit --cluster=yarn --num-workers=5 --num-servers=2  build/difacto task=train data_in=hdfs:///user/vinceywang/data/agaricus.txt.train model_in=hdfs:///user/vinceywang/model/difacto  model_out=hdfs:///user/vinceywang/model/difacto learner=sgd max_num_epochs=3 batch_size=100 V_dim=0 l1=0 l2=0 has_aux=1

#dmlc-core/tracker/dmlc-submit --cluster=yarn --num-workers=100 --num-servers=50  build/difacto task=train data_in=hdfs:///user/vinceywang/train_data model_in=hdfs:///user/vinceywang/model/difacto model_out=hdfs:///user/vinceywang/model/difacto learner=sgd max_num_epochs=3 batch_size=1000 V_dim=4 l1=0 l2=0 V_threshold=0 has_aux=0 num_jobs_per_epoch=100

dmlc-core/tracker/dmlc-submit --cluster='yarn' --num-workers=50 --num-servers=50  build/difacto yarn.conf

