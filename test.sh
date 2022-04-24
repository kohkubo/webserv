#!/bin/zsh

webserv="./webserv"
test="./intergration_test/integration_test"

(./webserv > /dev/null 2>&1) &
webserv_pid=`ps | grep webserv | sed '/grep/d' | awk '{print $1}'`

make run -C ./intergration_test 

kill $webserv_pid