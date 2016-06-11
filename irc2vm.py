#! /usr/bin/env python2

from VMHandler import IrcVMHandler, WebVMHandler

import threading

class AsyncThread(threading.Thread):

    def __init__(self, bot):
        self.bot = bot
        super(AsyncThread,self).__init__()
        self.daemon = True
        
    def run(self):
        self.bot.start()
            

def main():
    import sys
    if len(sys.argv) < 5:
        print("Usage: "+str(sys.argv[0])+" <server[:port]> <channel> <nickname> <VM name> [<out video device> <width:height:fps> <http listen port>]")
        sys.exit(1)

    s = sys.argv[1].split(":", 1)
    server = s[0]
    if len(s) == 2:
        try:
            port = int(s[1])
        except ValueError:
            print("Error: Erroneous port.")
            sys.exit(1)
    else:
        port = 6667
        
    channel = sys.argv[2]
    nickname = sys.argv[3]
    vm_name = sys.argv[4]
    vidDevName = None
            
    if len(sys.argv) >= 6:
        vidDevName = sys.argv[5]

    kwargs = {"width": 800, "height": 600, "fps": 25}
    if len(sys.argv) >= 7:
        s = sys.argv[6].split(":",3)
        kwargs["width"]=int(s[0])
        kwargs["height"]=int(s[1])
        kwargs["fps"]=int(s[2])
        

    irc_vm_handler = IrcVMHandler(vm_name, channel, nickname, server, vidDevName, port, **kwargs)

    if len(sys.argv) >= 8:
        http_port = int(sys.argv[7])
        web_vm_handler = WebVMHandler(vm_name, http_port, **kwargs)
        web_vm_handler.daemon=True
        web_vm_handler.start()

    irc_vm_handler.start()

    
if __name__ == "__main__":
    main()
