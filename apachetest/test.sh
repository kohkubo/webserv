set -x

#GET
export confpath="./conf/httpd.conf"
docker-compose up -d
curl localhost:8080 -v
docker-compose down

#cgi
export confpath="./conf/cgi.conf"
docker-compose up -d
curl -v --raw 'localhost:8080/cgi-bin/cgi_test/simple.py?name=taro'
curl -v --raw localhost:8080/cgi-bin/cgi_test/status.py
curl -v --raw localhost:8080/cgi-bin/cgi_test/add_header.py
docker-compose down