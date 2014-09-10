jarvinen
========

apt-get install libpcre3-dev

gcc -o jarvinen jarvinen.c -lpthread -lpcre

./jarvinen -l sample.log -x ids.rules -s apache -t 20
