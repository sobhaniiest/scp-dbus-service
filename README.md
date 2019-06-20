# scp-dbus-service
This repository is created for my Google Summer of Code'19 project with The Linux Foundation. 
The project title is - Turn the scp-dbus-service of system-config-printer into C. 
(https://wiki.linuxfoundation.org/gsoc/google-summer-code-2019-openprinting-projects)

# Interface : NewPrinterDialog & ConfigPrinting
Here I have used the D-Bus Introspection XML to autogenerate the the entire DBus interfaces in C - I have simply define the interface details (methods and signals) in an xml file and the gdbus-codegen (D-Bus code and documentation generator for DBus interface) is used to generate the .c and .h files for the DBus interface.

To generate the .c and .h files of the interfaces, simply run the command : "$ make gen" in the terminal window this will run the gdbus-codegen command with the appropriate flags on org.fedoraproject.Config.Printing.xml

Here the generated .c and .h files are 
- newprinterdialog_dbus.c 
- newprinterdialog_dbus.h
- Bus_name : org.fedoraproject.Config.Printing
- Interface : org.fedoraproject.Config.Printing & org.fedoraproject.Config.Printing.NewPrinterDialog

# Testing ::

Here the programs are able to export the methods of the NewPrinterDialog and ConfigPrinting and able to emit the signals of NewPrinterDialog. 

# Steps ::

1. make gen
2. make
3. ./main
4. in another terminal window :                                                                                               
dbus-send --session \                                                                                                         
          --dest=org.fedoraproject.Config.Printing \                                                                         
          --print-reply=literal \                                                                                             
          /org/fedoraproject/Config/Printing \                                                                               
          --type=method_call \                                                                                               
          org.fedoraproject.Config.Printing.NewPrinterDialog                                                                 
5. make clean

# Testing the methods of NewPrinterDialog Interface :

@dbus.service.method(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE, in_signature='uss', out_signature='')
def NewPrinterFromDevice(self, xid, device_uri, device_id):

dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.NewPrinterFromDevice \
          uint32:1 \
          string:'file:/tmp/printout' \
          string:'MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;'

@dbus.service.method(dbus_interface=CONFIG_NEWPRINTERDIALOG_IFACE, in_signature='us', out_signature='')
def DownloadDriverForDeviceID(self, xid, device_id):

dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.DownloadDriverForDeviceID \
          uint32:1 \
          string:'MFG:HP;MDL:hp 910;DES:hp 910;'


# Testing of scp-dbus-service(original) using dbus-send command ::
https://github.com/sobhaniiest/scp-dbus-service/blob/master/dbus-send.txt

