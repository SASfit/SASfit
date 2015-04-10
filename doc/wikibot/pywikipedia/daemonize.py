# -*- coding: utf-8  -*-
#
# (C) Pywikipedia bot team, 2007-2008, 2010
#
# Distributed under the terms of the MIT license.
#
__version__ = '$Id: daemonize.py 8633 2010-10-09 22:37:43Z xqt $'
#

import sys, os

is_daemon = False

def daemonize(close_fd=True, chdir=True, write_pid=False, redirect_std=None):
    """ Daemonize the current process. Only works on POSIX compatible operating
        systems. The process will fork to the background and return control to
        the terminal.

        Arguments:
            - close_fd: Close the standard streams and replace them by /dev/null
            - chdir: Change the current working directory to /
            - write_pid: Write the pid to sys.argv[0] + '.pid'
            - redirect_std: Filename to redirect stdout and stdin to
    """

    # Fork away
    if not os.fork():
        # Become session leader
        os.setsid()
        # Fork again to prevent the process from acquiring a
        # controlling terminal
        pid = os.fork()
        if not pid:
            global is_daemon
            is_daemon = True

            if close_fd:
                os.close(0)
                os.close(1)
                os.close(2)
                os.open('/dev/null', os.O_RDWR)
                if redirect_std:
                    os.open(redirect_std, os.O_WRONLY | os.O_APPEND)
                else:
                    os.dup2(0, 1)
                os.dup2(1, 2)
            if chdir:
                os.chdir('/')
            return
        else:
            # Write out the pid
            f = open(os.path.basename(sys.argv[0]) + '.pid', 'w')
            f.write(str(pid))
            f.close()
            os._exit(0)
    else:
        # Exit to return control to the terminal
        # os._exit to prevent the cleanup to run
        os._exit(0)
