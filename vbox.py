#! /usr/bin/env python2

from scancodes import *
import subprocess
from vboxapi import VirtualBoxManager

from time import sleep

import os,sys
import traceback

class VMHandler:
    def __init__(self, vm_name):
        self.vm_name = vm_name

        # This is a VirtualBox COM/XPCOM API client, no data needed.
        self.wrapper = VirtualBoxManager(None, None)

        # Get the VirtualBox manager
        mgr  = self.wrapper.mgr
        # Get the global VirtualBox object
        vbox = self.wrapper.vbox

        print "Running VirtualBox version %s" %(vbox.version)

        # Get all constants through the Python wrapper code
        self.vboxConstants = self.wrapper.constants

        self.mach = vbox.findMachine(vm_name)
        
        # Do some stuff which requires a running VM
        if self.mach.state == self.vboxConstants.MachineState_Running:
    
            # Get the session object
            self.session = mgr.getSessionObject(vbox)

            # Lock the current machine (shared mode, since we won't modify the machine)
            self.mach.lockMachine(self.session, self.vboxConstants.LockType_Shared)

            # Acquire the VM's console and guest object
            console = self.session.console
            self.mouse = console.mouse
            self.keyboard = console.keyboard


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

        if tks[0] == 'MOUSELC':
            self.handle_mouselc_command()

        if tks[0] == 'MOUSERC':
            self.handle_mouserc_command()

        if tks[0] == 'MOUSEMC':
            self.handle_mousemc_command()

        if tks[0] == 'MOUSEDC':
            self.handle_mousedc_command()

                
    def handle_type_command(self, c):

        for i, tk in enumerate(c):
            try:
                self.keyboard.putScancodes(type_in(tk,qwerty))
                
            except KeyError as ke:
                print("No corresponding scancode(s) to type'" + tk +"'.")

                
    def handle_mousemove_command(self, c):

        tks = c.split(" ")
        if len(tks) != 2:
            print "Wrong number of parameters for MOUSEMOVE."
            return

        try:
            
            dx = int(tks[0])
            dy = int(tks[1])

            self.mouse.putMouseEvent(dx, dy, 0, 0, 0x00)
            
        except ValueError:
            print "Cannot parse MOUSEMOVE parameters as integers."


    def handle_mouseset_command(self, c):

        tks = c.split(" ")
        if len(tks) != 2:
            print "Wrong number of parameters for MOUSEMOVE."
            return

        try:
            
            dx = int(tks[0])
            dy = int(tks[1])

            self.mouse.putMouseEventAbsolute(dx, dy, 0, 0, 0x00)
            self.mouse.putMouseEvent(0, 0, 0, 0, 0x00) # need this so the mouse stays visible
            
        except ValueError:
            print "Cannot parse MOUSESET parameters as integers."


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
