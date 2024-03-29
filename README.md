# [See Shiva2 , a new completely rewritten version here](https://github.com/IU2MEH/shiva2) https://github.com/IU2MEH/shiva2

## Shiva DX Cluster for SWL / BCL

Shiva DX Cluster connects to a ham radio dx cluster and repeat the dx spot to shiva clients while adding spots based on list files.

It downloads also Priyom.org number station schedules to show them dinamically on cluster softwares.

It doesn't send to ham radio dx cluster any spot nor it can receive any spot.

It is intended to use with SDR Console for SWL/BCL ( see screenshots folder ) so they are able to see broadcasting stations, for example, based on scheduling day and time.

Ham radio dx cluster spots are repeated to shiva clients immediately, broadcasting stations and other lists are sent to shiva clients every 10 minutes (default,configurable).

Priyom.org number stations are sent for 1 hour since the start of transmission.

See also "considerations" below

     ---------------------          -------
    |  Priyom.org         |------> |       |
     ---------------------         | shiva |
	 -----------------------       |       |        ---------------
  	| Ham radio DX cluster  | ---> |       | --->  | shiva clients |
	 -----------------------        -------         ---------------
         	                            |
                	                    |
                        	 ----------------------------
                          	| hfcc list and custom lists |
                        	 ----------------------------


## Getting started

#### If you want to access my shiva cluster already running h24, please contact me https://iu2meh.blogspot.com/

#### I will give you user and password if:

####	- you contribute to this project improving the code ( specially for known bugs or todo below )
	
####	- you contribute in another way like sending me a list or improving this doc or something like
	
####	- you send me little money via paypal to help to cover my server costs

	
	
Or you can run your own shiva dx cluster:

First, download executable (compiled for linux x64).

### Installing
Make sure Shiva executable has permission to execute:

chmod +x Shiva

Create a directory /etc/shiva/ as root

where put the config file shiva.conf and edit it as follows:



    timeout= timeout in milliseconds for tcp sockets

    callsign= callsign used to present shiva to public dx cluster, i prefer to use a faked one

	dxcluster=
	
	dxclusterport= are the address and port of the dx cluster to connect to

	listenport= tcp port where shiva listens for connections that comes from clients

	workingdir= directory where there are lists

	interval= interval time in milliseconds between list sendings



Download an updated broadcast scheduling list file from http://www.hfcc.org/data/

Actual list is "A20allx2.zip - A20 Operational Schedule - Last updated on 26-May-2020"

Unzip it in /etc/shiva/ (or another folder as you specified in conf file as workingdir ) and rename the main list ( in our example A20all00.TXT) as LIST.list

You can add other custom list files naming them with suffix .list, see below for the correct format.

Create a user.conf file with users and passwords coma separated, for example:

iu2meh,thisisthepassword

see folder config_samples and list_samples



### Usage
Simply run ./Shiva as normal user, you can pass --with-debug option to see debug messages.

It will connect to ham radio dx cluster and listen on a tcp port as specified in config file

Then you can connect to it with SDR Console https://www.sdr-radio.com/ or similar software or whatever can use a "classic" dx cluster

You can see here, in the screenshots folder, how to use it with SDR Console.

## Considerations

When you configure shiva cluster in SDR Console there is a field named "Display spots for: xxx minutes"

you have to tune it!

for example, you set it to 30 minutes, with shiva sending lists every 10 mins (default):

shiva send spot coming from list files every 10 minutes, so , if a station stop to tx at 20:00 in the worst case disappear from the sent list to you at 20:10 and from SDR Console waterfall at 20:40

on the other hand, if a station start to tx at 19:00, in the worst case shiva will update your waterfall at 19:10

so you tend to choose "Display spots for: 5 minutes" ( or less )

now in the worst case when a station stop to tx at 20:00 disappear from waterfall at 20:15, not bad, and when start for example at 19:00 you will see it at 19:05

but ham spots coming from the ham radio dx cluster are not always spotted every 5 mins, if you choose a short time these can appear/disappear repeatedly on your waterfall. 

It is good to use a short time in "Display spots for: xxx minutes" when working like an swl and a long time when working as ham radio operator.

## Custom list file

You can create a custom list file ( and please share it to me, it will be published here ) creating a new file with suffix .list in the workingdir.

Fields to be specified in the file are the followings, coma separated:

start_date,end_date,day_of_the_week,start_time,end_time,frequency,modulation,station_name



	start_date: day when the station will start to transmit in the format ddmmyyyy
	
	end_date: day when the station will stop to tx in the format ddmmyyyy, 31122100 is taken as standard for forever.
	
	day_of_the_week: day of the week when the station is active, 1 for monday 7 for sunday, for example if you write 123467 means that is active from monday to sunday except friday
	
	start_time:
	
	end_time: start and end time in UTC of tx, in the format hhmm, if the station transmits h24 you have to write 0000,2359 . Do NOT use 2400 as midnight
	
	frequency: self explanatory, in Hertz
	
	modulation: AM or LSB or USB or whatever you want
	
	station_name: self explanatory


	
Example:

28052020,31122100,1234567,0000,2359,252000,AM,Chaine 3 (AL)

means:  Chaine 3 from Algeria is transmitting on 252kHz in AM 24 hours a day everyday. (year 2100 means forever)

If you want to add an yearly event you can use the following notation:

17062020,1706****,1234567,0000,2359,999000,AM,test

means: station "test" will transmit on 999kHz AM for 24 hours every 17th of June.

I want to repeat... please SHARE your list files with this project and their users sending it to me.

## Build with
*   Qt Creator 4.12.1

    Based on Qt 5.14.2 (GCC 5.3.1 20160406 (Red Hat 5.3.1-6), 64 bit

    Built on May 19 2020 04:15:49

## Authors

* **Walter Panella IU2MEH** - *Initial work*

## Known Bugs
##### ( due to my little knowledge of QT and C :) )

### sometimes, when a shiva client disconnects, shiva cluster get the following error and crash:

QObject::connect: Cannot queue arguments of type 'QAbstractSocket::SocketError'

(Make sure 'QAbstractSocket::SocketError' is registered using qRegisterMetaType().)


## ToDo:

- Connect to reverse beacon network

- Don't read list files everytime but read one time and store it in an array and reread if file change
