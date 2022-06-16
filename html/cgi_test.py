#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

print("name=", form.getvalue("name", "default"))
print("blood=", form.getvalue("blood", "default"))
print("text=", form.getvalue("text", "default"))
