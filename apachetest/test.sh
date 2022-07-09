set -x

#ゲット
export confpath="./conf/httpd.conf"
docker-compose up -d
curl localhost:8080/html/ -v
docker-compose down
