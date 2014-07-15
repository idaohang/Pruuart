Pruuart
=======

Read ascii data from ADAFRUIT Ultimate GPS with the BBB PRUSS uart


Files:

pruuart-00A0.dts - devicetree for BBB Angstrom

pu.c             - initializes the PRUSS, reads data from local PRUSS memory, displays data

pu.p             - initializes the PRUSS uart, reads ascii data from ADAFRUIT Ultimate GPS, reads line until nl

notes            - a few notes about hardware setup & these programs
