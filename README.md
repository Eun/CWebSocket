Description
===========
This project has the aim to helps your project making http(s) calls, without any "big" libary.

CWebsocket is a small lightweight solution for unix and windows systems.

If you need anything or have questions please let me know!

How to use?
-----------
Include it into your CPP project

see example.php

Link with -lssl -lcrypto



Changelog
---------
VERSION 1.6:
* added https support via openssl
* removed opt.HEADERS

Version 1.5:

* changed COKIEFILE to COOKIEFILE
* added cleanup method
* changed internal constans
* added cleanup for cookiemanager
* added bind to interface

Version 1.4:

* changed from malloc to new

Version 1.3:

* fixed output overflow

Version 1.2:

* Added outputfile option
* using byte copys now 

Version 1.1:

* Added Content-Length Header

TODO
----
* windows openssl support
* cookiefile support       
* http 1.1 + unchunk
* auth
* HEADER output