# scp-dbus-service
This repository is created for my Google Summer of Code'19 project with The Linux Foundation. 
The project title is - Turn the scp-dbus-service of system-config-printer into C. 
(https://wiki.linuxfoundation.org/gsoc/google-summer-code-2019-openprinting-projects)

Here is the basic implementation of NewPrinterDialog interface (ConfigPrintingNewPrinterDialog.c). Here I have used the D-Bus Introspection XML to autogenerate the the entire DBus interface in C - I have simply define the interface details (methods and signals) in an xml file and the gdbus-codegen (D-Bus code and documentation generator for DBus interface) is used to generate the .c and .h files for the DBus interface.

To generate the .c and .h files of the NewPrinterDialog interface, simply run the command : $ make gen in the terminal window this will run the gdbus-codegen command with the appropriate flags on com.test.xml

Here the generated .c and .h files are 
- newprinterdialog_dbus.c 
- newprinterdialog_dbus.h
- Bus_name : com.test
- Interface : com.test.Printing

Testing ::

Here the programs are able to export the methods of the NewPrinterDialog interface and the methods can be invoked or called from another program (let's say a client, here this is test.c). 

First run the commend : $ make to generate the object files, then to export the methods of the interface run the command : $ ./main and to invoke a method (here it is ChangePPD of the NewPrinterDialog interface) run the command : $ ./demo following three command line attributes as the arguments of the ChangePPD method - xid(unsigned int), name(string), device_id(string).

This testing is only for methods exporting and invoking not the signals (working on this) and does not fulfil the functionality of the methods as I have not implemented the C code of the newprinter module.

Steps ::

1. make gen
2. make
3. ./main
4. ./demo xid name device_id (in a new terminal window)
5. make clean

Testing of scp-dbus-service(original) using dbus-send command ::
https://github.com/sobhaniiest/scp-dbus-service/blob/master/dbus-send.txt

