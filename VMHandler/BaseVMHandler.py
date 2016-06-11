#! /usr/bin/env python2

from vboxapi import VirtualBoxManager
from v4l2 import *
import numpy as np
import fcntl

from time import sleep
import threading

import os,sys
import traceback

from subprocess import Popen
           

class BaseVMHandler(object):
    def __init__(self, vm_name, vid_dev = None, grabber_path="./VboxGrabber/bin/VboxGrabber", width=1024, height=768, fps=25):
        
        self.vm_name = vm_name
        self.vid_dev = vid_dev
        
        self.width=width
        self.height=height
        self.fps=fps

        self.grabber_path=grabber_path

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
        if self.vid_dev != None:

            self.vmDesktopCapture = Popen([self.grabber_path, self.vm_name, self.vid_dev, str(self.width), str(self.height), str(self.fps)])


    def __del__(self):

        self.vmDesktopCapture.terminate();
        
        if self.mach.state == self.vboxConstants.MachineState_Running:
            # We're done -- don't forget to unlock the machine!
            self.session.unlockMachine()
        
            # Call destructor and delete wrapper
            del self.wrapper

        
