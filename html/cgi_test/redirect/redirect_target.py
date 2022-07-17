#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()

print("Status: 200 OK")
print("Content-type: text/html")
print("")
print("test:", form.getvalue("test", "default"))