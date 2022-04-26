#!/bin/zsh

webserv="./webserv"
testdir="./webtest"

($webserv > /dev/null 2>&1) &
webserv_pid=`ps | grep webserv | sed '/grep/d' | awk '{print $1}'`

make run -C $testdir

kill $webserv_pid