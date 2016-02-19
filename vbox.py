#! /usr/bin/env python3

from scancodes import *
import subprocess
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
                

class VMHandler:
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

        progress = self.mach.launchVMProcess(self.session, "headless", "")
        progress.waitForCompletion(-1)

        # Our session object is ready at this point, the machine is already locked
        # since we launched it ourselves
        
        if self.mach.state == self.vboxConstants.MachineState_PoweredOff:
            self.session.console.powerUp()

        while self.mach.state != self.vboxConstants.MachineState_Running:
            sleep(0.1)
        
        console = self.session.console

        self.mouse = console.mouse
        self.keyboard = console.keyboard
        self.display = console.display

        # Check if we have a v4l2 device to write to
        if vidDevName != None:
            
            self.device = open(vidDevName, 'w', 0) # Do not forget the 0 for unbuffered mode RGB(A) won't work otherwise

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
        
        
    def handle_command(self, c):
        
        tks = c.split(" ",1)
        
        if tks[0] == 'TYPE':
            self.handle_type_command(tks[1])

        if tks[0] == 'MOUSEMOVE':
            self.handle_mousemove_command(tks[1])

        if tks[0] == 'MOUSESET':
            self.handle_mouseset_command(tks[1])

        if tks[0] == 'LC':
            self.handle_mouselc_command()

        if tks[0] == 'RC':
            self.handle_mouserc_command()

        if tks[0] == 'MC':
            self.handle_mousemc_command()

        if tks[0] == 'DC':
            self.handle_mousedc_command()

        if tks[0] == 'ML':
            self.handle_ml_command()

        if tks[0] == 'MR':
            self.handle_mr_command()

        if tks[0] == 'MU':
            self.handle_mu_command()

        if tks[0] == 'MD':
            self.handle_md_command()

        if tks[0] == 'MLL':
            self.handle_mll_command()

        if tks[0] == 'MRR':
            self.handle_mrr_command()

        if tks[0] == 'MUU':
            self.handle_muu_command()

        if tks[0] == 'MDD':
            self.handle_mdd_command()

        if tks[0] == 'MHOLD':
            self.handle_mousehold_command()

        if tks[0] == 'MRELEASE':
            self.handle_mouserelease_command()

        if tks[0] == 'SHOLD':
            self.handle_shifthold_command()

        if tks[0] == 'SRELEASE':
            self.handle_shiftrelease_command()

        if tks[0] == 'AHOLD':
            self.handle_althold_command()

        if tks[0] == 'ARELEASE':
            self.handle_altrelease_command()

        if tks[0] == 'CHOLD':
            self.handle_ctrlhold_command()

        if tks[0] == 'CRELEASE':
            self.handle_ctrlrelease_command()

        if tks[0] == 'ENTER':
            self.handle_enter_command()

        if tks[0] == 'SPACE':
            self.handle_space_command()

        if tks[0] == 'RET':
            if len(tks) > 1:
                self.handle_return_command(tks[1])
            else:
                self.handle_return_command()
                
    def handle_type_command(self, c):

        for i, tk in enumerate(c):
            try:
                self.keyboard.putScancodes(type_in(tk,qwerty))
                
            except KeyError as ke:
                print("No corresponding scancode(s) to type '" + tk +"'.")

                
    def handle_mousemove_command(self, c):

        tks = c.split(" ")
        if len(tks) != 2:
            print("Wrong number of parameters for MOUSEMOVE.")
            return

        try:
            
            dx = int(tks[0])
            dy = int(tks[1])

            self.mouse.putMouseEvent(dx, dy, 0, 0, 0x00)
            
        except ValueError:
            print("Cannot parse MOUSEMOVE parameters as integers.")


    def handle_mouseset_command(self, c):

        tks = c.split(" ")
        if len(tks) != 2:
            print("Wrong number of parameters for MOUSESET.")
            return

        try:
            
            dx = int(tks[0])
            dy = int(tks[1])

            self.mouse.putMouseEventAbsolute(dx, dy, 0, 0, 0x00)
            self.mouse.putMouseEvent(0, 0, 0, 0, 0x00) # need this so the mouse stays visible
            
        except ValueError:
            print("Cannot parse MOUSESET parameters as integers.")


    def handle_mouselc_command(self):

        self.mouse.putMouseEvent(0, 0, 0, 0, 0x01)
        self.mouse.putMouseEvent(0, 0, 0, 0, 0x00)


    def handle_mouserc_command(self):

        self.mouse.putMouseEvent(0, 0, 0, 0, 0x02)
        self.mouse.putMouseEvent(0, 0, 0, 0, 0x00)

        
    def handle_mousemc_command(self):

        self.mouse.putMouseEvent(0, 0, 0, 0, 0x04)
        self.mouse.putMouseEvent(0, 0, 0, 0, 0x00)


    def handle_mousedc_command(self):

        self.mouse.putMouseEvent(0, 0, 0, 0, 0x01)
        self.mouse.putMouseEvent(0, 0, 0, 0, 0x00)
        sleep(0.05)
        self.mouse.putMouseEvent(0, 0, 0, 0, 0x01)
        self.mouse.putMouseEvent(0, 0, 0, 0, 0x00)

        
    def handle_ml_command(self):
        self.mouse.putMouseEvent(-3, 0, 0, 0, self.mouse_btns)
        

    def handle_mr_command(self):
        self.mouse.putMouseEvent(3, 0, 0, 0, self.mouse_btns)
        

    def handle_mu_command(self):
        self.mouse.putMouseEvent(0, -3, 0, 0, self.mouse_btns)
        

    def handle_md_command(self):
        self.mouse.putMouseEvent(0, 3, 0, 0, self.mouse_btns)


    def handle_mll_command(self):
        self.mouse.putMouseEvent(-15, 0, 0, 0, self.mouse_btns)
        

    def handle_mrr_command(self):
        self.mouse.putMouseEvent(15, 0, 0, 0, self.mouse_btns)
        

    def handle_muu_command(self):
        self.mouse.putMouseEvent(0, -15, 0, 0, self.mouse_btns)
        

    def handle_mdd_command(self):
        self.mouse.putMouseEvent(0, 15, 0, 0, self.mouse_btns)


    def handle_mousehold_command(self):
        self.mouse_btns = 0x01
        self.mouse.putMouseEvent(0, 0, 0, 0, self.mouse_btns)


    def handle_mouserelease_command(self):
        self.mouse_btns = 0x00
        self.mouse.putMouseEvent(0, 0, 0, 0, self.mouse_btns)

        
    def handle_shifthold_command(self):
        self.keyboard.putScancodes([L_SHIFT])


    def handle_shiftrelease_command(self):
        self.keyboard.putScancodes([L_SHIFT | 0x80])
        

    def handle_althold_command(self):
        self.keyboard.putScancodes([L_ALT])
        

    def handle_altrelease_command(self):
        self.keyboard.putScancodes([L_ALT | 0x80])
        

    def handle_ctrlhold_command(self):
        self.keyboard.putScancodes([L_CTRL])


    def handle_ctrlrelease_command(self):
        self.keyboard.putScancodes([L_CTRL | 0x80])


    def handle_enter_command(self):
        self.keyboard.putScancodes([ENTER])
        self.keyboard.putScancodes([ENTER | 0x80])

        
    def handle_space_command(self):
        self.keyboard.putScancodes([SPACE])
        self.keyboard.putScancodes([SPACE | 0x80])
        
        
    def handle_return_command(self, c=None):

        if c == None:
            self.keyboard.putScancodes([BACKSPACE])
            self.keyboard.putScancodes([BACKSPACE | 0x80])
        
            return
        
        tks = c.split(" ")
        if len(tks) == 1:
        
            try:
            
                num = int(tks[0])

                for i in range(0,num):
                    self.keyboard.putScancodes([BACKSPACE])
                    self.keyboard.putScancodes([BACKSPACE | 0x80])
            
            except ValueError:
                print("Cannot parse RET parameters as integers.")
            
        
