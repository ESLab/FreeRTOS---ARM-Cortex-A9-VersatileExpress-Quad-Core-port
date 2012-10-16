This is the FreeRTOS port for the Versatile Express using the CoreTile 9x4 Daughterboard equipped with the ARM Cortex-A9MPCore Quad-Core CPU.

Authors: 	Dag Ågren, Åbo Akademi University, Finland
			Simon Holmbacka, Åbo Akademi University, Finland
 
This port has been developed as a part of the RECOMP project in Åbo Akademi University, Finland
http://research.it.abo.fi/
Contact email: sholmbac@abo.fi

This port was tested on the aforementioned platform with 4 identical FreeRTOS instances, one on each core.

::Compilation::
This project has been built with the devkitARM (release 37) gcc compiler version 4.6.2
http://sourceforge.net/projects/devkitpro/files/devkitARM/

Go to the Demo folder and type 'make' to compile the example project.
4 different binaries will be created:
FreeRTOSDemo.0.bin
FreeRTOSDemo.1.bin
FreeRTOSDemo.2.bin
FreeRTOSDemo.3.bin

::Set up tftp::
Install a tftp server on your computer for example
http://sourceforge.net/projects/atftp/

Move FreeRTOSDemo.*.bin to your tftp root folder and rename the binary files to:
0.bin
1.bin
2.bin
3.bin
(The U-Boot on the Versatile cannot handle long file names)

Go to the VersatileLoader folder and copy l.bin to your tftp root folder

::Load images onto the Versatile Express::
Power up the Versatile Express
Go into the boot monitor and set the server ip to the ip address of your computer running tftp with:
	setenv serverip <ip address>
	saveenv
Set the boot commands for your build images with:
	setenv bootcmd 'dhcp 0x0010000 0.bin; dhcp 0x1010000 1.bin; dhcp 0x2010000 2.bin; dhcp 	0x3010000 3.bin; dhcp 0x800 l.bin;go 0x800'
The demo should run on all four cores
The demo prints out a core identifier on the serial ports of the VersatileExpress

::MISC::
The Demo enables L1 D and I cache on all cores.
MMU is enabled with a page table defined in the port.c file.
L2 cache is disabled.
The memory entry points for each CPU are defined in the VersatileLoader folder in the startup.S file.


