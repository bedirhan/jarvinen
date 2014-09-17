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
At first, ids.xml file should be downloaded from at https://github.com/PHPIDS/PHPIDS/blob/master/lib/IDS/default_filter.xml

./jarvinen -t 10 -l sample.log -f default_filter.xml

Sample output
-------------

xss,csrf - /wp-content/plugins/sociable/js/sociable.js?ver=3.4.1 - X.X.X.X - 15/Sep/2012:20:45:39
xss,csrf - /wp-content/plugins/sociable/js/addtofavorites.js?ver=3.4.1 - X.X.X.X - 15/Sep/2012:20:45:39



ToDo
 * logging
 * geoip
 * nginx
 * iss
