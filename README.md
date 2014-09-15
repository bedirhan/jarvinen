jarvinen
========

#### Installation
####### apt-get install libpcre3-dev libxml2-dev geoip-database libgeoip-dev

#### Compile
####### gcc jarvinen.c -o jarvinen -lpthread -lpcre -I/usr/include/libxml2 -lxml2

#### Usage 
####### ./jarvinen -l sample.log -x ids.rules -s apache -t 20

#### ToDo
 * logging
 * geoip
 * nginx
 * iss
