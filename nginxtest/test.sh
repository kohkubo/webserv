set -x
export confpath="./conf/nginx.conf"
docker-compose up -d
curl localhost:8080 -v
docker-compose down

export confpath="./conf/delete.conf"
mkdir deletedir
echo "##\nin delete.html\n##" > deletedir/delete.html
echo "##\nin index.html\n##" > deletedir/index.html
docker-compose up -d
curl localhost:8080/delete.html -v
curl -X DELETE localhost:8080/delete.html -v
curl localhost:8080/delete.html -v
echo "##########"
curl localhost:8080 -v
curl -X DELETE localhost:8080 -v
curl localhost:8080 -v
docker-compose down
