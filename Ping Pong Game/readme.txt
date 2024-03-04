Assuming mcb32 toolchain and a C Compiler are installed
How to compile and run the code in a UNIX-based terminal:

. /opt/mcb32tools/environment
make
make install TTYDEV=/dev/ttyUSB0  // This may need to be adjusted depending on OS or USB-port