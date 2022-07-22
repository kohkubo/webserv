#!/bin/bash

make
clang++ client.cpp -o bad_client
./webserv conf/webserv.conf &
pid=$!
./bad_client
ps pid
if [ $? -eq 0 ]; then
  kill -9 pid
else
  exit 1
fi
exit 0