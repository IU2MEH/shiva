# Shiva DX Cluster for SWL / BCL

This software connect to a dx cluster and repeat the dx spot to its clients while adding spot about broadcasting stations on shortwave or anything you want to add.

It doesn't send to public cluster any spot nor it can receive any spot

It is intended to use with SDR Console or similar software for SWL/BCL ( see screenshots folder )

Dx cluster spots are repeated to shiva clients immediately, broadcasting stations and other lists are sent to shiva clients every 10 minutes

See also "considerations" below


       -------------        -------         ---------------
      | DX cluster  | ---> | shiva | --->  | shiva clients |
       -------------        -------         ---------------
                               |
                               |
                  ----------------------------
                 | hfcc list and custom lists |
                  ----------------------------


## Getting started
Download executable (compiled for linux x64).

### Installing
Make sure Shiva executable has permission to execute:

chmod +x Shiva

Create a directory /etc/shiva/ as root

where put the config file shiva.conf and edit it as follows:



        timeout= timeout for tcp sockets

        callsign= callsign used to present shiva to public dx cluster, i prefer to use a faked one

	dxcluster=
	
	dxclusterport= are the address and port of the dx cluster to connect to

	listenport= tcp port where shiva listens for connections that comes from clients

	workingdir= directory where there are lists



Download an updated broadcast scheduling list from http://www.hfcc.org/data/

Actual list is "A20allx2.zip - A20 Operational Schedule - Last updated on 26-May-2020"

Unzip it in /etc/shiva/ (or another folder as you specified in conf file as workingdir ) and rename the main list ( in our example A20all00.TXT) as LIST.list

You can add other custom lists naming them with suffix .list, see below for correct format.

### Usage
Simply run ./Shiva as normal user.

It will connect to dx cluster and listen on tcp port as specified in config file

Then you can connect to it with SDR Console https://www.sdr-radio.com/ or similar software or whatever can use a "classic" dx cluster

You can see here, in the screenshots folder, how to use it with SDR Console.

## Build with
*   Qt Creator 4.12.1

    Based on Qt 5.14.2 (GCC 5.3.1 20160406 (Red Hat 5.3.1-6), 64 bit

    Built on May 19 2020 04:15:49

## Authors

* **Walter Panella IU2MEH** - *Initial work*

## Known Bugs
##### ( due to my little knowledge of QT and C :) )
### when someone connects:

QObject: Cannot create children for a parent that is in a different thread.

(Parent is Sons(0x560c9e5be3f0), parent's thread is QThread(0x560c9e5ba4a0), current thread is Sons(0x560c9e5be3f0)

QObject: Cannot create children for a parent that is in a different thread.

(Parent is QNativeSocketEngine(0x7f3f24003b70), parent's thread is Sons(0x560c9e5be3f0), current thread is QThread(0x560c9e5ba4a0)

### sometimes, while sending lines to clients:

QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread

### sometimes, when someone disconnects, shiva cluster get the following error and crash:

QObject::connect: Cannot queue arguments of type 'QAbstractSocket::SocketError'

(Make sure 'QAbstractSocket::SocketError' is registered using qRegisterMetaType().)


## Considerations

When you configure shiva cluster in SDR Console there is a field named "Display spots for: xxx minutes"

you have to tune it!

for example, you set it to 30 minutes:

shiva send spot coming from list every 10 minutes, so , if a broadcast station stop to tx at 20:00 in the worst case disappear from the sent list to you at 20:10 and from SDR Console waterfall at 20:40

on the other hand, if a station start to tx at 19:00, in the worst case shiva will update your waterfall at 19:10

so you tend to choose "Display spots for: 5 minutes" ( or less )

now in the worst case when station stop to tx at 20:00 disappear from waterfall at 20:15 not bad, and when start for example at 19:00 you will see it at 19:05

but ham spots coming from the dx cluster are not always spotted every 5 mins, if you choose a short interval these can appear/disappear repeatedly

It is good to use a short interval in "Display spots for: xxx minutes" when working like an swl and a long interval when working as ham radio operator.
