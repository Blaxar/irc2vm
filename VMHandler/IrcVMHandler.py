#! /usr/bin/env python2

from scancodes import *
from BaseVMHandler import BaseVMHandler

import irc
import irc.bot
import irc.strings
from irc.client import ip_numstr_to_quad, ip_quad_to_numstr

try:
    import irc.buffer as ircbuf
except ImportError:
    import jaraco.stream.buffer as ircbuf #for older versions of irc module

irc.client.ServerConnection.buffer_class = ircbuf.LenientDecodingLineBuffer #To take care of invalid utf-8 codes

class IrcVMHandler(irc.bot.SingleServerIRCBot, BaseVMHandler):

    def __init__(self, vm_name, channel, nickname, server, vid_dev = None, port=6667, **kwargs):
        BaseVMHandler.__init__(self, vm_name, vid_dev, **kwargs)
        irc.bot.SingleServerIRCBot.__init__(self, [(server, port)], nickname, nickname)
        self.channel = channel
        self.command_str = "VMBOT"

    def on_nicknameinuse(self, c, e):
        c.nick(c.get_nickname() + "_")

    def on_welcome(self, c, e):
        c.join(self.channel)

    def on_privmsg(self, c, e):
        nick = e.source.nick
        text = e.arguments[0]
        c.notice(nick, text)
        #self.do_command(e, e.arguments[0])

    def on_pubmsg(self, c, e):
        self.handle_command(e.arguments[0])

    def on_dccmsg(self, c, e):
        # non-chat DCC messages are raw bytes; decode as text
        text = e.arguments[0].decode('utf-8')
        c.privmsg("You said: " + text)

    def on_dccchat(self, c, e):
        if len(e.arguments) != 2:
            return
        args = e.arguments[1].split()
        if len(args) == 4:
            try:
                address = ip_numstr_to_quad(args[2])
                port = int(args[3])
            except ValueError:
                return
            self.dcc_connect(address, port)

    def do_command(self, e, cmd):
        nick = e.source.nick
        c = self.connection

        if cmd == "disconnect":
            self.disconnect()
        elif cmd == "die":
            self.die()
        elif cmd == "stats":
            for chname, chobj in self.channels.items():
                c.notice(nick, "--- Channel statistics ---")
                c.notice(nick, "Channel: " + chname)
                users = sorted(chobj.users())
                c.notice(nick, "Users: " + ", ".join(users))
                opers = sorted(chobj.opers())
                c.notice(nick, "Opers: " + ", ".join(opers))
                voiced = sorted(chobj.voiced())
                c.notice(nick, "Voiced: " + ", ".join(voiced))
        elif cmd == "dcc":
            dcc = self.dcc_listen()
            c.ctcp("DCC", nick, "CHAT chat %s %d" % (
                ip_quad_to_numstr(dcc.localaddress),
                dcc.localport))
        else:
            c.notice(nick, "Not understood: " + cmd)
            
    def handle_command(self, c):
        
        tks = c.split(" ",1)
        
        if tks[0] == 'TYPE':
            if len(tks) == 2:
                self.handle_type_command(tks[1])

        if tks[0] == 'MOUSEMOVE':
            if len(tks) == 2:
                self.handle_mousemove_command(tks[1])

        if tks[0] == 'MOUSESET':
            if len(tks) == 2:
                self.handle_mouseset_command(tks[1])

        if tks[0] == 'LC':
            self.handle_mouselc_command()

        if tks[0] == 'RC':
            self.handle_mouserc_command()

        if tks[0] == 'MC':
            self.handle_mousemc_command()

        if tks[0] == 'DC':
            self.handle_mousedc_command()

        if tks[0] == 'MX':
            if len(tks) == 2:
                self.handle_mx_command(tks[1])

        if tks[0] == 'MY':
            if len(tks) == 2:
                self.handle_my_command(tks[1])
            
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

            self.mouse.putMouseEvent(dx, dy, 0, 0, self.mouse_btns)
            
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

            self.mouse.putMouseEventAbsolute(dx, dy, 0, 0, self.mouse_btns)
            self.mouse.putMouseEvent(0, 0, 0, 0, self.mouse_btns) # need this so the mouse stays visible
            
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


    def handle_mx_command(self, c):
        
        tks = c.split(" ")
        if len(tks) != 1:
            print("Wrong number of parameters for MX.")
            return

        try:

            self.mouse.putMouseEvent(int(tks[0]), 0, 0, 0, self.mouse_btns)
            
        except ValueError:
            print("Cannot parse MX parameters as integers.")


    def handle_my_command(self, c):
        
        tks = c.split(" ")
        if len(tks) != 1:
            print("Wrong number of parameters for MY.")
            return

        try:
            self.mouse.putMouseEvent(0, int(tks[0]), 0, 0, self.mouse_btns)
            
        except ValueError:
            print("Cannot parse MY parameters as integers.")
        
        
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

        

        
