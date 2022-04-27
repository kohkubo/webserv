#!/bin/zsh

webserv="./webserv"
testdir="./webtest"

# バックグラウンドでwebserv実行(出力が邪魔するので/dev/nullへ)
($webserv > /dev/null 2>&1) &

# webservのプロセスIDを取得
# ps | grep webserv で取得できるのは以下２つあるので, sedとか使ってます.
#  1. webservプロセス
#  2. webservをgrepするプロセス
webserv_pid=`ps | grep webserv | sed '/grep/d' | awk '{print $1}'`

# webtestディレクトリ内のgoのテストを走らせる
make run -C $testdir

# webservを終了させる
kill $webserv_pid
