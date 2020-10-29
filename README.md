AISLANDI  is a TCP gateway for a AIS receiver with RS-232 interface.
The automatic identification system (AIS) is an automatic tracking system 
that uses transceivers on ships and is used by vessel traffic services (VTS). 
AIS is intended to assist a vessel's watchstanding officers and allow maritime 
authorities to track and monitor vessel movements.
The program works on any flavour of Linux, anyway it has been built for
Raspberry PI (2,3,4).
The AIS module tested is a modern AIS receiver with USB/Rs-232 Interface:
https://shop.wegmatt.com/collections/frontpage/products/daisy-2-dual-channel-ais-receiver-with-nmea-0183
The gateway decodes standard NMEA-00183 message, so it should work with any
compatible AIS module.
This module is part of project for a drone boat,anyway it's usable for any
different purpose where an AIS receiver is required.

Authors: Achille and Samuele Landi

Configuration:
Edit the file ais-landi-server.c and change the following line, replacing
/dev/ttyACM0 with the serial port where the AIS is wired:
#define RS232 "/dev/ttyACM0"

Requirements:
gcc compiler only, for Raspberry PI you could install the following package:
apt-get install build-essential

Build:
build the program executing from the folder of download
cd libs/aisparser/c/so
make linux
ldconfig
make linux
ldconfig
cd ../../../../
./compile.sh

Run:
./ais-landi-server


The gateway will listen on port 8001/tcp for a connection. As soon a
connection arrives,it send the last know vessels positions in json format and
disconnects. For example:
{"mmsi":"378111129","latitude":"25.118295","longitude":"55.141885"}
{"mmsi":"235115083","latitude":"25.117493","longitude":"55.138837"}
Where mssi is the vessel unique id.

You can try to connect to the gateway from the same device with:
telnet 127.0.0.1 8001

Additional information are shown on the screen and written to the syslog file.

For any help, please drop an email to: samuele@landi.ae or achille@landi.ae












