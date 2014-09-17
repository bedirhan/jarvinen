jarvinen
========

#### Installation
####### apt-get install libpcre3-dev libxml2-dev

#### Compile
####### gcc jarvinen.c -o jarvinen -lpthread -lpcre -I/usr/include/libxml2 -lxml2

#### Usage 
####### ./jarvinen -t 10 -l sample.log -f ids.rules 

#### ToDo
 * logging
 * geoip
 * nginx
 * iss
