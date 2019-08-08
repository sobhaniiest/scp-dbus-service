# scp-dbus-service
This repository is created for my Google Summer of Code'19 project with The Linux Foundation. 
The project title is - Turn the scp-dbus-service of system-config-printer into C. 
(https://wiki.linuxfoundation.org/gsoc/google-summer-code-2019-openprinting-projects)

# scp-service (C libraries with API) 
This component can be found here - https://github.com/sobhaniiest/SCP_Libraries

# GDBus Interface :
Here I have used the D-Bus Introspection XML to autogenerate the the entire DBus interfaces in C - I have simply define the interface details (methods and signals) in an xml file and the gdbus-codegen (D-Bus code and documentation generator for DBus interface) is used to generate the .c and .h files for the DBus interface.

Here the generated .c and .h files are 
- scp_interface.c 
- scp_interface.h

# Testing : 
To prevent the functionality of the old scp-dbus-service.py, cd to the directory "/usr/bin/" and open the bash file "scp-dbus-service", comment on the lines and restart the system.


# Build ::

$ autoreconf -fi                                                                                                             
$ ./configure                                                                                                               
$ make                                                                                                                       

# Testing the interfaces using dbus-send :

$ cd dbus                                                                                                                   
$ ./scp                                                                                                                     

- Open a new terminal window and run the following dbus-send commands to test the methods and interfaces                     

# ConfigPrinting :

- method : NewPrinterDialog                                       
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.NewPrinterDialog

- method : MissingExecutables(ppd_filename)                       
dbus-send --session \                      
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.MissingExecutables \
          string:'Generic-PDF_Printer-PDF.ppd'
                                                                                                                             
- method : GetBestDrivers(device_id, device_make_and_model, device_uri)                                              
dbus-send --session \                                                                                                       
          --dest=org.fedoraproject.Config.Printing \                                                                         
          --print-reply=literal \                                                                                           
          /org/fedoraproject/Config/Printing \                                                                               
          --type=method_call \                                                                                               
          org.fedoraproject.Config.Printing.GetBestDrivers \                                                                 
          string:'MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;' \             
          string:'Generic PDF Printer' \                                                                                     
          string:'file:/tmp/printout'                                                                                       
          
# NewPrinterDialog

- method : NewPrinterFromDevice (xid, device_uri, device_id)      
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.NewPrinterFromDevice \
          uint32:1 \
          string:'file:/tmp/printout' \
          string:'MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;'

- method : DownloadDriverForDeviceID (xid, device_id)             
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.DownloadDriverForDeviceID \
          uint32:1 \
          string:'MFG:HP;MDL:hp 910;DES:hp 910;'

- method : ChangePPD(xid, name, device_id)                        
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/NewPrinterDialog/1 \
          --type=method_call org.fedoraproject.Config.Printing.NewPrinterDialog.ChangePPD \
          uint32:1 \
          string:'test' \
          string:'MFG:Generic;CMD:PJL,PDF;MDL:PDF Printer;CLS:PRINTER;DES:Generic PDF Printer;DRV:DPDF,R1,M0;' 

# ConfigPrintingPrinterPropertiesDialog

- method : PrinterPropertiesDialog(xid, name)                                                                              
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing \
          --type=method_call \
          org.fedoraproject.Config.Printing.PrinterPropertiesDialog \
          uint32:1 \
          string:'test'

- method : PrintTestPage                                                                                                     
dbus-send --session \
          --dest=org.fedoraproject.Config.Printing \
          --print-reply=literal \
          /org/fedoraproject/Config/Printing/PrinterPropertiesDialog/1 \
          --type=method_call \
          org.fedoraproject.Config.Printing.PrinterPropertiesDialog.PrintTestPage
