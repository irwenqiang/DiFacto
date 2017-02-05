#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/data/clusterserver/hadoop/lib/native/
build/difacto local.conf data_in=data/rcv1_train.binary learner=sgd  V_dim=2 max_num_epochs=5 batch_size=100 has_aux=1 model_out = "mymodel"
