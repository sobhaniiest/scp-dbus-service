'''
The general idea is that new instances of the New Printer and Printer Properties dialogs, 
as well as a single instance of the Job Applet, can be requested, and these are given their own object paths.  
Each has its own simple interface.

Interface: org.fedoraproject.Config.Printing.NewPrinterDialog

Two methods and three signals :: 

Method NewPrinterFromDevice
Show a dialog for creating a new printer, given a device URI and an IEEE 1284 Device ID.

Method ChangePPD
Show a dialog for changing the PPD for an existing printer.

Signal PrinterAdded
The printer has now been added.

Signal PrinterModified
The PPD has now been changed.

Signal DialogCanceled
The dialog has been cancelled by the user.

A base class for exporting your own Objects across the Bus.

Just inherit from Object and mark exported methods with the @dbus.service.method or @dbus.service.signal decorator.

'''

#ConfigPrintingNewPrinterDialog - derived class inherits features from the dbus.service.Object - base class
class ConfigPrintingNewPrinterDialog(dbus.service.Object):
    # Constructors - line 485 
    def __init__ (self, bus, path, cupsconn): 
        # CONFIG_BUS='org.fedoraproject.Config.Printing', bus - dbus.SessionBus() 
        # same as class ConfigPrinting(dbus.service.Object): 
        bus_name = dbus.service.BusName (CONFIG_BUS, bus=bus)
        '''
        A base class for exporting your own Named Services across the Bus. When instantiated, objects of this class attempt
        to claim the given well-known name on the given bus for the current process. The name is released when the BusName 
        object becomes unreferenced. If a well-known name is requested multiple times, multiple references to the same 
        BusName object will be returned.
        '''
        '''
        CONFIG_PATH='/org/fedoraproject/Config/Printing'
        line - 482
        def NewPrinterDialog(self):    # Creates a new instance of the New Printer dialog and returns its object path.
        self._pathn += 1
        path = "%s/NewPrinterDialog/%s" % (CONFIG_PATH, self._pathn)
        ConfigPrintingNewPrinterDialog (self.bus, path,
                                        self._cupsconn)
        g_killtimer.alive ()
        return path
        '''
        dbus.service.Object.__init__ (self, bus_name, path) # constructor of dbus.service.Object
        self.dialog = newprinter.NewPrinterGUI() # creating an object of the NewPrinterGUI class in newprinter module
        self.dialog.NewPrinterWindow.set_modal (False) # can not find
        # Signals ?????
        self.handles = [self.dialog.connect ('dialog-canceled',
                                             self.on_dialog_canceled),
                        self.dialog.connect ('printer-added',
                                             self.on_printer_added),
                        self.dialog.connect ('printer-modified',
                                             self.on_printer_modified),
                        self.dialog.connect ('driver-download-checked',
                                             self.on_driver_download_checked)]
        self._ppdcache = ppdcache.PPDCache () # creating an object of the PPDCache class in ppdcache module
        #self._cupsconn = asyncconn.Connection () : creating an object of the class Connection in the asyncconn module
        self._cupsconn = cupsconn
        debugprint ("+%s" % self)

    def __del__ (self):
        self.dialog.destroy ()
        debugprint ("-%s" % self)

    @dbus.service.method(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE,
                         in_signature='uss', out_signature='')
    # Show a dialog for creating a new printer, given a device URI and an IEEE 1284 Device ID.
    # The IEEE 1284 Device ID is used to identify the connected printer and its capabilities. 
    # Some printers also provide status information via the ID string.
    def NewPrinterFromDevice(self, xid, device_uri, device_id):
        g_killtimer.add_hold ()
        self.dialog.init ('printer_with_uri', device_uri=device_uri,
                          devid=device_id, xid=xid)

    @dbus.service.method(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE,
                         in_signature='us', out_signature='')
    def DownloadDriverForDeviceID(self, xid, device_id):
        g_killtimer.add_hold ()
        self.dialog.init ('download_driver', devid=device_id, xid=xid)

    @dbus.service.method(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE,
                         in_signature='uss', out_signature='')
    # Show a dialog for changing the PPD for an existing printer.
    def ChangePPD(self, xid, name, device_id):
        g_killtimer.add_hold ()
        self.xid = xid
        self.name = name
        self.device_id = device_id
        self._ppdcache.fetch_ppd (name, self._change_ppd_got_ppd)

    def _change_ppd_got_ppd(self, name, ppd, exc):
        # Got PPD; now find device URI.
        self.ppd = ppd
        self._cupsconn.getPrinters (reply_handler=self._change_ppd_with_dev,
                                    error_handler=self._do_change_ppd)

    def _change_ppd_with_dev (self, conn, result):
        self.device_uri = result.get (self.name, {}).get ('device-uri', None)
        self._do_change_ppd (conn)

    def _do_change_ppd(self, conn, exc=None):
        self.dialog.init ('ppd', device_uri=self.device_uri, name=self.name,
                          ppd=self.ppd, devid=self.device_id, xid=self.xid)

    @dbus.service.signal(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE,
                         signature='')
    # The dialog has been cancelled by the user.
    def DialogCanceled(self):
        pass

    @dbus.service.signal(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE,
                         signature='s')
    # The printer has now been added.
    def PrinterAdded(self, name):
        pass

    @dbus.service.signal(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE,
                         signature='sb')
    # The PPD has now been changed.
    def PrinterModified(self, name, ppd_has_changed):
        pass

    @dbus.service.signal(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE,
                         signature='a(s)')
    def DriverDownloadChecked(self, installed_files):
        pass

    def on_dialog_canceled(self, obj):
        debugprint ("%s: dialog canceled" % self)
        g_killtimer.remove_hold ()
        self.DialogCanceled ()
        self.remove_handles ()
        self.remove_from_connection ()

    def on_printer_added(self, obj, name):
        debugprint ("%s: printer added" % self)
        g_killtimer.remove_hold ()
        self.PrinterAdded (name)
        self.remove_handles ()
        self.remove_from_connection ()

    def on_printer_modified(self, obj, name, ppd_has_changed):
        debugprint ("%s: printer modified" % self)
        g_killtimer.remove_hold ()
        self.PrinterModifed (name, ppd_has_changed)
        self.remove_handles ()
        self.remove_from_connection ()

    def on_driver_download_checked(self, obj, installed_files):
        debugprint ("%s: driver download checked" % self)
        g_killtimer.remove_hold ()
        self.DriverDownloadChecked (installed_files)
        self.remove_handles ()
        self.remove_from_connection ()

    def remove_handles (self):
        for handle in self.handles:
            self.dialog.disconnect (handle)


