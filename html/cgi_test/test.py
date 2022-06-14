#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

print("HTTP/1.0 200 OK")
print("Content-type: text/html\n\n")
print("")
print("Hello world")
print("<p>name:", form.getvalue("name", "default"))
print("<p>sex:", form.getvalue("sex", "default"))
print("<p>blood:", form.getvalue("blood", "default"))
print("<p>end</p>")

cgi.test()
