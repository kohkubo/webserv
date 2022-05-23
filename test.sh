echo
echo "======================"
echo "normal 5000"
echo "======================"
curl localhost:5000
echo
echo "======================"
echo "normal 5000 404"
echo "======================"
curl localhost:5000/jlksdfjklfsdjkldfs
echo
echo "======================"
echo "5001 403"
echo "normal 5000"
echo "======================"
chmod 000 html/index.html
curl localhost:5000 -v
chmod 744 html/index.html
echo
echo "======================"
echo "get request path"
echo "normal 5000 in dir1"
echo "======================"
curl localhost:5000/dir1/index2.html
echo
echo "======================"
echo "cgi"
echo "normal 5000 cgi"
echo "======================"
curl localhost:5000/cgi.sh
echo
echo "======================"
echo "root directive test"
echo "normal 5001 in dir1"
echo "======================"
curl localhost:5001
echo
echo "======================"
echo "index directive test"
echo "normal 5002 index2! in dir1"
echo "======================"
curl localhost:5002
echo
echo "======================"
echo "error_page directive test 404"
echo "normal 5003"
echo "======================"
curl localhost:5003/dlkdsklsdf
