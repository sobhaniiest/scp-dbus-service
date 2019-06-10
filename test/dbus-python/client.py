#!/usr/bin/env python

import sys
from traceback import print_exc

import dbus

def main():
    bus = dbus.SessionBus()

    a = input("num1 : ")
    b = input("num2 : ")
    try:
        remote_object = bus.get_object("com.test.CalculatorService", "/Calculate")

        # you can either specify the dbus_interface in each call...
        s = remote_object.add(int(a), int(b), dbus_interface = "com.test.CalculatorInterface")
    except dbus.DBusException:
        print_exc()
        sys.exit(1)

    print("sum = ", s)

    remote_object = bus.get_object("com.test.CalculatorService", "/Calculate")

    # ... or create an Interface wrapper for the remote object
    iface = dbus.Interface(remote_object, dbus_interface = "com.test.CalculatorInterface")

    s = iface.sub(int(a), int(b))
    print("sub = ", s)

    s = iface.multi(int(a), int(b))
    print("multi = ", s)

    # D-Bus exceptions are mapped to Python exceptions
    try:
        iface.RaiseException()
    except dbus.DBusException as e:
        print(str(e))

    # introspection is automatically supported
    print(remote_object.Introspect(dbus_interface="org.freedesktop.DBus.Introspectable"))

    if sys.argv[1:] == ['--exit']:
        iface.Exit()

if __name__ == '__main__':
    main()