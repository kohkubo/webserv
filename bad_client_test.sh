#!/bin/bash

make
clang++ client.cpp -o bad_client
./webserv conf/webserv.conf &
pid=$!
echo $pid
./bad_client
ps pid
ret=$?
if [ ret -eq 0 ]; then
  kill -9 pid
  exit 0
else
  exit 1
fi
