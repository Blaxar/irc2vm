#! /usr/bin/env python2

from scancodes import *
from BaseVMHandler import BaseVMHandler
import threading
import socket
import re

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
            # Look in the first line of the request for a move command
            # A move command should be e.g. 'http://server/move?a=90'
            #match = re.match('POST.*HTTP/1', req)
            #if match:
            match_args = re.search('arg\=(.*)$', req)
            if match_args:
                tks = match_args.group(1).split(",")
                print(tks)
                if len(tks) == 5:
                    try:
                        self.mouse_btns = 0x00
                        if int(tks[2]) > 0: self.mouse_btns = self.mouse_btns | 0x01
                        if int(tks[3]) > 0: self.mouse_btns = self.mouse_btns | 0x04
                        if int(tks[4]) > 0: self.mouse_btns = self.mouse_btns | 0x02

                        width, height, bitsPerPixel, xOrigin, yOrigin, guestMonitorStatus = self.display.getScreenResolution(0) 
                        
                        self.mouse.putMouseEventAbsolute(int(float(tks[0])*width), int(float(tks[1])*height), 0, 0, self.mouse_btns)
                        self.mouse.putMouseEvent(0, 0, 0, 0, self.mouse_btns) # need this so the mouse stays visible

                        csock.sendall("""HTTP/1.0 200 OK""")
                        
                    except TypeError as ve:
                        print(ve)
    

    def __del__(self):
        self.sock.shutdown(socket.SHUT_RDWR)
        self.sock.close()
