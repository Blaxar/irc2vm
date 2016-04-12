#! /usr/bin/env python2

from scancodes import *
from BaseVMHandler import BaseVMHandler
import threading
import socket
import re

import tornado.websocket

# Inspired by this: http://blog.scphillips.com/posts/2012/12/a-simple-python-webserver/

class WebVMHandler(BaseVMHandler, threading.Thread):

    class WSHandler(tornado.websocket.WebSocketHandler):
        
        def open(self):
            print("WebSocket opened")

        def check_origin(self, origin):
            return True

        def on_message(self, message):
            self.write_message(u"You said: " + message)

        def on_close(self):
            print("WebSocket closed")
            
    
    def __init__(self, vm_name, port = 8080, vidDevName = None):
        
        BaseVMHandler.__init__(self, vm_name, vidDevName)
        threading.Thread.__init__(self)
        self.port = port

        self.app = tornado.web.Application([(r"/", WebVMHandler.WSHandler)])
        self.app.listen(self.port)

        
    def run(self):
        
        tornado.ioloop.IOLoop.current().start()
        
"""
class WebVMHandler(BaseVMHandler, threading.Thread):

    def __init__(self, vm_name, port = 8080, vidDevName = None):
        BaseVMHandler.__init__(self, vm_name, vidDevName)
        threading.Thread.__init__(self)
        
        # Standard socket stuff:
        self.host = '' # do we need socket.gethostname() ?
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.bind((self.host, self.port))
        self.sock.listen(1) # don't queue up any requests

    def run(self):
        # Loop forever, listening for requests:
        while True:
            csock, caddr = self.sock.accept()
            print("Connection from: " + `caddr`)
            req = csock.recv(1024) # get the request, 1kB max
            print(req)
            match_args = re.search('GET /\?arg\=(.*) HTTP/1', req)
            if match_args:
                tks = match_args.group(1).split(",")
                #print(tks)
                if len(tks) == 5:
                    try:
                        self.mouse_btns = 0x00
                        if int(tks[2]) > 0: self.mouse_btns = self.mouse_btns | 0x01
                        if int(tks[3]) > 0: self.mouse_btns = self.mouse_btns | 0x04
                        if int(tks[4]) > 0: self.mouse_btns = self.mouse_btns | 0x02

                        width, height, bitsPerPixel, xOrigin, yOrigin, guestMonitorStatus = self.display.getScreenResolution(0) 
                        
                        self.mouse.putMouseEventAbsolute(int(float(tks[0])*width), int(float(tks[1])*height), 0, 0, self.mouse_btns)
                        self.mouse.putMouseEvent(0, 0, 0, 0, self.mouse_btns) # need this so the mouse stays visible

                        csock.send(r'''HTTP/1.0 200 OK
                        Content-Type: text/plain

                        ''')
                        
                    except TypeError as ve:
                        print(ve)
    

    def __del__(self):
        self.sock.shutdown(socket.SHUT_RDWR)
        self.sock.close()
"""
