echo
echo "======================"
echo "normal 5000"
echo "======================"
curl localhost:5000
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
echo "normal 5001 404"
echo "======================"
curl localhost:5000/jlksdfjklfsdjkldfs
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
