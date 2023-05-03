#!/bin/bash

set -x

protoc -I ../proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ../proto/database.proto
protoc -I ../proto --cpp_out=. ../proto/database.proto