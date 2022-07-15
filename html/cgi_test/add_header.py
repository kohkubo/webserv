#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

# cgiのレスポンスの改行は\n(UNIX,POSIX共通)
print("Status: 200 OK")
print("Content-type: text/html")
print("hoge: fuga")
print("dummy-header: dududu")
print("")
print("Hello world", end="")