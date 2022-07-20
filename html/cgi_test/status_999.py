#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

# cgiのレスポンスの改行は\n(UNIX,POSIX共通)
print("Status: 999 INVALID STATUS")
print("Content-type: text/html")
print("")
print("Hello world")
print("<p>name:", form.getvalue("name", "default"))
print("<p>sex:", form.getvalue("sex", "default"))
print("<p>blood:", form.getvalue("blood", "default"))
print("<p>end</p>")