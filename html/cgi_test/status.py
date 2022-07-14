#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

# cgiのレスポンスの改行は\n(UNIX,POSIX共通)
print("Status: 418 hoge")
print("Content-type: text/html")
print("")
print("Hello world")