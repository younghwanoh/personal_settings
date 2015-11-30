#!/bin/bash

# extract ~/Packages/0.1.1.tar.gz .
# mv bazel-0.1.1 bazel
# cd bazel
# ./compile.sh

git clone https://github.com/bazelbuild/bazel.git
cd bazel
git checkout tags/0.1.1
./compile.sh
export PATH=$PATH:/home/yhlinux/dnn/bazel/output

git clone --recurse-submodules https://github.com/tensorflow/tensorflow
cd tensorflow
# TF_CUDA_COMPUTE_CAPABILITIES=3.0 TF_UNOFFICIAL_SETTING=1 TF_NEED_CUDA=1 CUDA_TOOLKIT_PATH=/usr/local/cuda-7.0 CUDNN_INSTALL_PATH=/usr/local/cuda-7.0 ./configure
TF_NEED_CUDA=0 ./configure
bazel build -c opt //tensorflow/cc:tutorials_example_trainer
bazel-bin/tensorflow/cc/tutorials_example_trainer

bazel build -c opt //tensorflow/tools/pip_package:build_pip_package
bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow_pkg
sudo pip install /tmp/tensorflow_pkg/tensorflow-0.5.0-cp27-none-linux_x86_64.whl
