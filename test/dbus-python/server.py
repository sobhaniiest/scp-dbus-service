#!/usr/bin/env python

from gi.repository import GLib

import dbus
import dbus.service
import dbus.mainloop.glib

class DemoException(dbus.DBusException):
    _dbus_error_name = "com.test.DemoException"

class Calculate(dbus.service.Object):

    @dbus.service.method("com.test.CalculatorInterface", in_signature='ii', out_signature='i')  
    def add(self, a, b):
        s = int(a) / int(b)
        print(int(a)," + ", int(b), " = ", s)
        return s

    @dbus.service.method("com.test.CalculatorInterface", in_signature='ii', out_signature='i')  
    def sub(self, a, b):
        s = int(a) - int(b)
        print(int(a)," - ", int(b), " = ", s)
        return s

    @dbus.service.method("com.test.CalculatorInterface", in_signature='ii', out_signature='i')  
    def multi(self, a, b):
        s = int(a) * int(b)
        print(int(a)," * ", int(b), " = ", s)
        return s

    @dbus.service.method("com.test.CalculatorInterface", in_signature='', out_signature='')
    def RaiseException(self):
        raise DemoException("The RaiseException method !!!")

    @dbus.service.method("com.test.CalculatorInterface", in_signature='', out_signature='')
    def Exit(self):
        mainloop.quit()

if __name__ == '__main__':

    # main loop to allow asynchronous method calls
    dbus.mainloop.glib.DBusGMainLoop(set_as_default = True)

    # connect to the session bus by creating a session bus object 
    session_bus = dbus.SessionBus()

    # A base class for exporting your own Named Services 
    # "com.test.CalculatorService" across the Bus.
    name = dbus.service.BusName("com.test.CalculatorService", session_bus)
    
    object = Calculate(session_bus, "/Calculate")

    print("Server is running !!!")

    # GLib will run the callbacks when appropriate
    mainloop = GLib.MainLoop()
    mainloop.run()
