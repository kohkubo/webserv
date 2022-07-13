#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

# cgiのレスポンスの改行は\n(UNIX,POSIX共通)
print("Status: 200 OK")
print("hoge: ", end="")
for num in range(21474836478888):
    print("a", end="")
print("")
print("Content-type: text/html")
print("")
print("hello world")