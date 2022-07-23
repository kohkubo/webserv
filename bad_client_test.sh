#!/bin/bash

set -eux

make
clang++ client.cpp -o bad_client
./webserv conf/webserv.conf &
pid=$!
sleep 2
./bad_client
ret=`ps | grep webserv | wc -l`
if [ $ret = 1 ]; then
  kill -9 $pid
  exit 0
fi
exit 1
