#!/usr/bin/python3
# -*- coding: utf-8 -*-


import cgi
import sys
import cgitb
import os

form = cgi.FieldStorage()

username = form.getvalue('username')
password = form.getvalue('password')

if username == 'admin' and password == 'admin':
    response = "HTTP/1.1 200 OK\r\n"
    response += "Content-Type: text/html\r\n\r\n"
    response += "<html><head><title>Login Success</title></head><body>\n"
    response += "<h1>Login Success</h1>\n"
    response += "<p>You are logged in as %s</p>\n" % username
    response += "<p>You are logged with password: %s</p>\n" % password
    response += "</body></html>\n"
    sys.stdout.write(response)
    exit(0)
elif username == 'exit' and password == 'exit':
    exit(-1)

