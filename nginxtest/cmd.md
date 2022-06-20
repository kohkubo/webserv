# up

docker-compose up -d

# stop

docker-compose down

# into container

docker exec -it nginx_test_web_1 bash

# conf file validate

nginx -t

# test

zsh test.sh &> log
