#! /usr/bin/env python2

from scancodes import *
from BaseVMHandler import BaseVMHandler

import web

class WebVMHandler(BaseVMHandler):

    def __init__(self, vm_name, vidDevName = None):
        super(WebVMHandler, self).__init__(vm_name, vidDevName)
        

        
