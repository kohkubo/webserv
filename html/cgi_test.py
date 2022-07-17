#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()
print("Status: 200 OK")
print("Content-type: text/html")
print("")
print("name=", form.getvalue("name", "default"))
print("blood=", form.getvalue("blood", "default"))
print("text=", form.getvalue("text", "default"))
