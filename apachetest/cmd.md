# up

docker-compose up -d

# stop

docker-compose down

# into container

docker exec -it apachetest_web_1 bash

# conf file validate

apachectl configtest

# test

zsh test.sh &> log
