#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

# cgiのレスポンスの改行は\n(UNIX,POSIX共通)
print("Status: 200 OK")
print("hoge: ", ("a" * 8183))
print("")
print("Content-type: text/html")
print("")
print("hello world")