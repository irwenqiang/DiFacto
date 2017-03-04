#export PS_VERBOSE=2

dmlc-core/tracker/dmlc-submit --cluster=yarn --num-workers=50 --num-servers=50  build/difacto yarn.conf

