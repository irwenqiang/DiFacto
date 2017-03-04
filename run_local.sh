export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/data/clusterserver/hadoop/lib/native/
build/difacto local.conf data_in=data/train_data1 data_val=data/train_data1 learner=sgd  V_dim=0 max_num_epochs=2 batch_size=1000 has_aux=1 model_in = "mymodel" l1_shrk=0
