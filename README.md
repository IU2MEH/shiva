# Shiva SWL DX Cluster

This software connect to a dx cluster and repeat the dx spot to clients connected to it while adding spot about broadcasting stations on shortwave or anything you want.

     -------------        -------         ---------
    | DX cluster  | ---> | shiva | --->  | clients |
     -------------        -------         ---------
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
   
   callsign= callsign used to present shiva to dx cluster, i prefer to use a faked one
   
   dxcluster=
   
   dxclusterport= are the address and port of the dx cluster to connect to
   
   listenport= tcp port where shiva listens for connections that comes from clients
   
   working dir= directory where there are lists
   
   

Download an updated broadcast scheduling list from http://www.hfcc.org/data/

Actual list is "A20allx2.zip - A20 Operational Schedule - Last updated on 26-May-2020"

Unzip it in /etc/shiva/ and rename the main list ( in our example A20all00.TXT) as LIST.list

You can add other custom lists naming them with suffix .list


### Running
Simply run ./Shiva as normal user.

## Build with
*   Qt Creator 4.12.1

    Based on Qt 5.14.2 (GCC 5.3.1 20160406 (Red Hat 5.3.1-6), 64 bit)
    
    Built on May 19 2020 04:15:49

## Authors

* **Walter Panella IU2MEH** - *Initial work*

## Known Bugs

### when someone connects:

QObject: Cannot create children for a parent that is in a different thread.
(Parent is Sons(0x560c9e5be3f0), parent's thread is QThread(0x560c9e5ba4a0), current thread is Sons(0x560c9e5be3f0)

QObject: Cannot create children for a parent that is in a different thread.

(Parent is QNativeSocketEngine(0x7f3f24003b70), parent's thread is Sons(0x560c9e5be3f0), current thread is QThread(0x560c9e5ba4a0)



### sometimes while sending lines to clients:

QSocketNotifier: Socket notifiers cannot be enabled or disabled from another thread



### sometimes when someone disconnects shiva cluster get the following error and hung:

QObject::connect: Cannot queue arguments of type 'QAbstractSocket::SocketError'
(Make sure 'QAbstractSocket::SocketError' is registered using qRegisterMetaType().)
