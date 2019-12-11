# GDBus Interface :
Here I have used the D-Bus Introspection XML to autogenerate the the entire DBus interfaces in C - I have simply defined the interface details (methods and signals) in an xml file and the gdbus-codegen (D-Bus code and documentation generator for DBus interface) is used to generate the .c and .h files for the DBus interface.

Here the generated .c and .h files are 
- scp_interface.c 
- scp_interface.h

# Dependencies :

- [CUPS] (https://github.com/apple/cups/releases) version >= 2.2.7
- GLIB-2.0
- libxml2
- libtool
- regex
- GTK+-3.0

# Testing : 
To prevent the functionality of the old scp-dbus-service.py, cd to the directory "/usr/bin/" and open the bash file "scp-dbus-service", comment on the lines and restart the system.

# Build and Installation  

```
$ autoreconf -fi                                                                                                       
$ ./configure                                                                                                         
$ make                                                                                                              
$ sudo make install                                                                                                   
$ sudo ldconfig 
```

# scp-service (C libraries with API) 
# Testing the libraries

```
$ cd demo/                                                                                                             
$ make                                                                                                                 
$ ./test 'xid' 'device_uri' 'device_id' 'printer_name' 'ppd_file_name' 
e.g : ./test 0 'file:/tmp/printout' 'MFG:HP;MDL:hp 910;DES:hp 910;' 'test' 'Generic-PDF_Printer-PDF.ppd'
```

# scp-dbus-service (exporting methods and signals using dbus)
# Testing the interfaces using dbus-send :
```
$ cd dbus                                                                            
$ ./scp                                                                              
```
- Open a new terminal window and run the following dbus-send commands to test the methods and interfaces.                     

# ConfigPrinting :

- method : NewPrinterDialog   
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.NewPrinterDialog
```
- method : MissingExecutables(ppd_filename)  
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.MissingExecutables \
          string:'Generic-PDF_Printer-PDF.ppd'
```  
- method : GetBestDrivers(device_id, device_make_and_model, device_uri) 
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.GetBestDrivers \
          string:'MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;' \
          string:'Generic PDF Printer' \
          string:'file:/tmp/printout'
```          
# NewPrinterDialog

- method : NewPrinterFromDevice (xid, device_uri, device_id)
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.NewPrinterFromDevice \
          uint32:1 \
          string:'file:/tmp/printout' \
          string:'MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;'
```
- method : DownloadDriverForDeviceID (xid, device_id)  
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.DownloadDriverForDeviceID \
          uint32:1 \
          string:'MFG:HP;MDL:hp 910;DES:hp 910;'
```
- method : ChangePPD(xid, name, device_id)  
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.ChangePPD \
          uint32:1 \
          string:'test' \
          string:'MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;' 
```
# ConfigPrintingPrinterPropertiesDialog

- method : PrinterPropertiesDialog(xid, name)
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.PrinterPropertiesDialog \
          uint32:1 \
          string:'test'
```
- method : PrintTestPage
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/PrinterPropertiesDialog/1 \
          --type=method_call \
          org.fedoraproject.Config.Printing.PrinterPropertiesDialog.PrintTestPage
```

# ConfigPrintingJobapplet

- method : JobApplet
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.JobApplet
```

- method : Quit
```
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/JobApplet/1 \
          --type=method_call \
          org.fedoraproject.Config.Printing.JobApplet.Quit
```
