#!/usr/bin/python3

import cgi
import os

form = cgi.FieldStorage()

# cgiのレスポンスの改行は\n(UNIX,POSIX共通)
print("Status: 200 OK")
print("Content-type: text/html")
print("")
print("Hello world")
dirpath = os.getcwd()
print("pwd:", dirpath[dirpath.rfind(os.sep) + 1:])