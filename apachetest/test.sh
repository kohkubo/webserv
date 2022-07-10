set -x

#GET
export confpath="./conf/httpd.conf"
docker-compose up -d
curl localhost:8080/html/ -v
docker-compose down

#cgi
export confpath="./conf/cgi.conf"
docker-compose up -d
curl http://localhost:8080/cgi-bin/first.pl
docker-compose down