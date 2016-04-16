#! /usr/bin/env python2

from scancodes import *
from BaseVMHandler import BaseVMHandler
import threading
import re

import tornado.websocket

class WebVMHandler(BaseVMHandler, threading.Thread):

    class WSHandler(tornado.websocket.WebSocketHandler):

        def initialize(self, display, mouse):
            self.display = display
            self.mouse = mouse
            
        def open(self):
            print("WebSocket opened")

        def check_origin(self, origin):
            return True

        def on_message(self, message):
            
            match_args = re.search('arg\=(.*)', message)
            
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
                        
                    except TypeError as ve:
                        print(ve)

        def on_close(self):
            print("WebSocket closed")
            
    
    def __init__(self, vm_name, port = 8080, vidDevName = None):
        
        BaseVMHandler.__init__(self, vm_name, vidDevName)
        threading.Thread.__init__(self)
        self.port = port

        self.app = tornado.web.Application([(r"/", WebVMHandler.WSHandler,
                                            dict(display=self.display,mouse=self.mouse))]) # we need to access display and keyboard from within the WS handler
        self.app.listen(self.port)
        
    def run(self):
        
        tornado.ioloop.IOLoop.current().start()
