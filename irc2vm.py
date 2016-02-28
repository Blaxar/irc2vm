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
        print("Usage: testbot <server[:port]> <channel> <nickname> <VM name> [<out video device> <http listen port>]")
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

    irc_vm_handler = IrcVMHandler(vm_name, channel, nickname, server, vidDevName, port)

    if len(sys.argv) >= 7:
        http_port = int(sys.argv[6])
        web_vm_handler = WebVMHandler(vm_name, http_port)
        web_vm_handler.daemon=True
        web_vm_handler.start()

    irc_vm_handler.start()

    
if __name__ == "__main__":
    main()
