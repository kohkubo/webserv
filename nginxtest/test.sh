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
rm -r deletedir

export confpath="./conf/post.conf"
mkdir postdir
docker-compose up -d
curl -d "content" -X PUT http://localhost:8080/filename -v
curl -d "update" -X PUT http://localhost:8080/filename -v
curl -d "content" -X PUT http://localhost:8080/postdir/filename -v
curl -d "content" -X PUT http://localhost:8080/nosuch/filename -v
curl -d "content" -X PUT http://localhost:8080/dir/ -v
docker-compose down
rm -r postdir

export confpath="./conf/autoindex.conf"
mkdir -p autoindexdir/dir1/dir2
echo "in autoindexdir/" > autoindexdir/test.html
echo "in autoindexdir/dir1/" > autoindexdir/dir1/index.html
echo "in autoindexdir/dir1/dir2/" > autoindexdir/dir1/dir2/test.html
ln -s autoindexdir/test.html autoindexdir/link_to_test
docker-compose up -d
curl localhost:8080/autoindexdir -v
curl localhost:8080/autoindexdir/ -v
curl localhost:8080/autoindexdir/dir1/ -v
curl localhost:8080/autoindexdir/dir1/dir2/ -v
rm -r autoindexdir
