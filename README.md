jarvinen
========

#### Installation

####### apt-get install libpcre3-dev geoip-database libgeoip-dev

#### Compile

####### gcc -o jarvinen jarvinen.c -lpthread -lpcre

#### Usage 

####### ./jarvinen -l sample.log -x ids.rules -s apache -t 20
