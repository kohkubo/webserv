make
./webserv conf/test.conf &
pid=$!

rm -rf output.txt
touch output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "normal 5000" >> output.txt
echo "======================" >> output.txt
curl localhost:5000 >> output.txt
echo >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "normal 5000/dir1" >> output.txt
echo "======================" >> output.txt
curl localhost:5000/dir1/ >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "normal 5000 404" >> output.txt
echo "======================" >> output.txt
curl localhost:5000/jlksdfjklfsdjkldfs >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "5001 403" >> output.txt
echo "normal 5000" >> output.txt
echo "======================" >> output.txt
chmod 000 html/index.html
curl localhost:5000 -v >> output.txt
chmod 744 html/index.html
echo >> output.txt
echo "======================" >> output.txt
echo "get request path" >> output.txt
echo "normal 5000 in dir1" >> output.txt
echo "======================" >> output.txt
curl localhost:5000/dir1/index2.html >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "cgi" >> output.txt
echo "normal 5000 cgi" >> output.txt
echo "======================" >> output.txt
curl localhost:5000/cgi.sh >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "root directive test" >> output.txt
echo "normal 5001 in dir1" >> output.txt
echo "======================" >> output.txt
curl localhost:5001 >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "index directive test" >> output.txt
echo "normal 5002 index2! in dir1" >> output.txt
echo "======================" >> output.txt
curl localhost:5002 >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "error_page directive test 404" >> output.txt
echo "normal 5003" >> output.txt
echo "======================" >> output.txt
curl localhost:5003/dlkdsklsdf >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "limit_except test" >> output.txt
echo "200 OK 5003" >> output.txt
echo "======================" >> output.txt
curl localhost:5003 >> output.txt
echo >> output.txt
echo "======================" >> output.txt
echo "limit_except test" >> output.txt
echo "405 error 5003" >> output.txt
echo "======================" >> output.txt
curl -X POST -d 'name=taro' localhost:5003/html/cgi.sh >> output.txt

kill $pid

diff -u output.txt expect.txt
if [ $? -eq 0 ]; then
    echo "OK"
    exit 0
else
    echo "NG"
    exit 1
fi
