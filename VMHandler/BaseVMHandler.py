#! /usr/bin/env python2

from vboxapi import VirtualBoxManager
from v4l2 import *
import numpy as np
import fcntl

from time import sleep
import threading

import os,sys
import traceback

class V4l2Capture(threading.Thread):

    def __init__(self, device, format, display, bmp_fmt):
        self.device = device
        self.format = format
        self.display = display
        self.bmp_fmt = bmp_fmt
        super(V4l2Capture,self).__init__()

    def run(self):

        while True:
            try:
                im = self.display.takeScreenShotToArray(0, self.format.fmt.pix.width, self.format.fmt.pix.height, self.bmp_fmt)
                img=np.fromstring(im)
                self.device.write(img)
                sleep(1./60.)
            except Exception as e:
                sleep(0.1)
                

class BaseVMHandler(object):
    def __init__(self, vm_name, vidDevName = None):
        
        self.vm_name = vm_name
        self.mouse_btns = 0x00

        # This is a VirtualBox COM/XPCOM API client, no data needed.
        self.wrapper = VirtualBoxManager(None, None)

        # Get the VirtualBox manager
        mgr  = self.wrapper.mgr
        # Get the global VirtualBox object
        vbox = self.wrapper.vbox

        print ("Running VirtualBox version %s" %(vbox.version))

        # Get all constants through the Python wrapper code
        self.vboxConstants = self.wrapper.constants

        self.mach = vbox.findMachine(vm_name)

                    # Get the session object
        self.session = mgr.getSessionObject(vbox)

        #If the machine is already Running
        if self.mach.state == self.vboxConstants.MachineState_Running:

            # Lock the current machine (shared mode, since we won't modify the machine)
            self.mach.lockMachine(self.session, self.vboxConstants.LockType_Shared)

        #Else, we have to launch the machine first
        else:

            progress = self.mach.launchVMProcess(self.session, "headless", "")
            progress.waitForCompletion(-1)

            # Our session object is ready at this point, the machine is already locked
            # since we launched it ourselves
        
            if self.mach.state == self.vboxConstants.MachineState_PoweredOff:
                self.session.console.powerUp()

            #We wait for the machine to be really running before doing anything else
            while self.mach.state != self.vboxConstants.MachineState_Running:
                sleep(0.1)
        
        console = self.session.console

        self.mouse = console.mouse
        self.keyboard = console.keyboard
        self.display = console.display

        # Check if we have a v4l2 device to write to
        if vidDevName != None:
            
            self.device = open(vidDevName, 'w', 0) # Do not forget the 0 for unbuffered mode RGB(A), won't work otherwise

            fmt = V4L2_PIX_FMT_BGR32
            
            width = 640
            height = 480
        
            self.format = v4l2_format()
            self.format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT
            self.format.fmt.pix.pixelformat = fmt
            self.format.fmt.pix.width = width
            self.format.fmt.pix.height = height
            self.format.fmt.pix.field = V4L2_FIELD_NONE
            self.format.fmt.pix.bytesperline = width * 4
            self.format.fmt.pix.sizeimage = width * height * 4
            self.format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB

            fcntl.ioctl(self.device, VIDIOC_S_FMT, self.format)

            self.vmDesktopCapture = V4l2Capture(self.device, self.format, self.display, self.vboxConstants.BitmapFormat_BGRA)
            self.vmDesktopCapture.daemon = True
            self.vmDesktopCapture.start() # Run the capturing thread


    def __del__(self):

        if self.mach.state == self.vboxConstants.MachineState_Running:
            # We're done -- don't forget to unlock the machine!
            self.session.unlockMachine()
        
            # Call destructor and delete wrapper
            del self.wrapper

        
