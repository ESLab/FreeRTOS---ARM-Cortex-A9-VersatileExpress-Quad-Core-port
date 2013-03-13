FreeRTOS---ARM-Cortex-A9-VersatileExpress-Quad-Core-port
========================================================



This port runs four separate FreeRTOS instances on the VersatileExpress with the CoreTile 9x4 Daughter board and the ARM Cortex-A9 MPCore Quad-Core CPU. Each serial port on the VersatileExpress prints out its related core ID as well as the Demo output of each instance of FreeRTOS.

Each core uses its L1 I and D cache and the MMU.Cached pages range from 0 to 64 MB (1 GB of memory in total) in the physical memory space. Shared memory can be used for core-to-core communication between the FreeRTOS instances in the non-cached memory.

For more information on how to compile and boot the FreeRTOS port on the VersatileExpress see the readme file.

This port was created by the Department of Information Technologies at Åbo Akademi University (https://research.it.abo.fi/) within the RECOMP project (http://www.recomp-project.eu/).

[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/2f005e48c99a4aeb9ef2a93ae6de0b68 "githalytics.com")](http://githalytics.com/ESLab/FreeRTOS---ARM-Cortex-A9-VersatileExpress-Quad-Core-port)


