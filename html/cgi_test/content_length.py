#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

# cgiのレスポンスの改行は\n(UNIX,POSIX共通)
print("Status: 200 OK")
print("Content-type: text/html")
print("Content-Length: 11")
print("")
print("Hello world")
print("hogehogehoge")