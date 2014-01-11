#!/usr/bin/env python

#
# Pebble Server Monitor - Host monitor and JSON server
#   Written by edenist
#   20140107
#
#



import psutil
import socket
import json
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

testJSON = {"Data":{"cpu":12,"mem":100,"host":"JSONServe"}}
jSysStats = {"Data":{"cpu":0,"mem":0,"host":"DEFAULT"}}

class JSONHandler (BaseHTTPRequestHandler):
  def do_GET(self):
    try:
      if self.path.endswith(".json"):
        cpu = psutil.cpu_percent()
        mem = psutil.virtual_memory()[2]
        host = socket.gethostname()
        jSysStats["Data"]["cpu"] = cpu
        jSysStats["Data"]["mem"] = mem
        jSysStats["Data"]["host"] = host
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(jSysStats))
        return
      return
    except IOError:
      self.send_error(404, 'File Not Found: %s' % self.path)

def main():
  try:
    server = HTTPServer(('', 8080), JSONHandler)
    print 'started JSON server'
    server.serve_forever()
  except KeyboardInterrupt:
    print '^C received, shutting down server'
    server.socket.close()

if __name__ == '__main__':
  main()
        
