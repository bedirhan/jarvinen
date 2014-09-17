jarvinen
========

Installation
------------

In order to run jarvinen, some dependency should be install. 

 apt-get install libpcre3-dev libxml2-dev

Compilation
-----------

gcc jarvinen.c -o jarvinen -lpthread -lpcre -I/usr/include/libxml2 -lxml2

Usage 
-----
./jarvinen -t 10 -l sample.log -f ids.rules 


ToDo
 * logging
 * geoip
 * nginx
 * iss
