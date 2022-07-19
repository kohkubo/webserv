#!/usr/bin/python3

print("Location: http://localhost:55000/cgi_test/redirect/target.py")
print("Status: 302 Found")

print("Content-Type: text/html")
print("dummy-header: dududu")
print("")
print("some content from rediroc response", end="")