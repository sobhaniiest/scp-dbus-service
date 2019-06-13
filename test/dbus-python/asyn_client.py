import sys
import traceback

from gi.repository import GLib

import dbus
import dbus.mainloop.glib

# Callbacks for asynchronous calls

def handle_add_reply(s):
    global add_replied
    add_replied = True

    print("sum = ", int(s))

    if add_replied and raise_replied:
        loop.quit()

def handle_add_error(e):
    global failed
    global add_replied
    hello_replied = True
    failed = True

    print("add raised an exception! That's not meant to happen...")
    print(str(e))

    if add_replied and raise_replied:
        loop.quit()

def handle_raise_reply():
    global failed
    global raise_replied
    raise_replied = True
    failed = True

    print("RaiseException returned normally! That's not meant to happen...")

    if add_replied and raise_replied:
        loop.quit()

def handle_raise_error(e):
    global raise_replied
    raise_replied = True

    print("RaiseException raised an exception as expected:")
    print(str(e))

    if add_replied and raise_replied:
        loop.quit()

def make_calls():
    # To make an async call, use the reply_handler and error_handler kwargs
    remote_object.add(sys.argv[1], sys.argv[2],
                    dbus_interfac='com.test.CalculatorInterface',
                    reply_handler=handle_add_reply,
                    error_handler=handle_add_error)

    # Interface objects also support async calls
    iface = dbus.Interface(remote_object, 'com.test.CalculatorInterface')
    
    iface.RaiseException(reply_handler=handle_raise_reply,
                         error_handler=handle_raise_error)

    return False

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default = True)

    bus = dbus.SessionBus()
    try:
        remote_object = bus.get_object("com.test.CalculatorService","/Calculate")
    except dbus.DBusException:
        traceback.print_exc()
        sys.exit(1)

    # Make the method call after a short delay
    GLib.timeout_add(1000, make_calls)

    failed = False
    hello_replied = False
    raise_replied = False

    loop = GLib.MainLoop()
    loop.run()
    if failed:
        raise SystemExit("test async client failed!")
