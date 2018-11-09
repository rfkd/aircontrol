# **aircontrol**

### **FEATURES**

* Send configurable radio frames with different encodings through a radio transmitter supported by [WiringPi](http://wiringpi.com/).
* Wireless scanning support in combination with a radio receiver for analyzing and replaying radio frames.
* Record and replay radio frames to easily control air targets without having to decode the protocol.
* Configuration file support for easily addressing air targets.


### **SUPPORTED DEVICES**

The following devices have been confirmed to work with aircontrol: 
* ELRO AB440S and many similar wireless outlets (encoding type: Remote Controlled Outlet)
* Tormatic garage doors (encoding type: Tormatic)
* Warema EWFS based devices, e.g. shutters (encoding type: Manchester)
* WOFI FC-66 controlled lamps (encoding type: Remote Controlled Outlet) 

If you are controlling other devices with aircontrol please let me know.


### **INSTALLATION**

aircontrol needs to be compiled on a Raspberry Pi (or on a host with a compatible cross tool chain). The following libraries are needed:

* **WiringPi**, see <http://wiringpi.com/download-and-install/>
* **libconfig++**, install with: `apt-get install libconfig++-dev`

Run the following command to build aircontrol:
> $ make

Complete the installation as root (optional):
> \# make install

The following command removes aircontrol and its configuration file:
> \# make uninstall

Please note that aircontrol needs to be executed as root for accessing the GPIO hardware.


### **COMMAND LINE PARAMETERS**

Execute `aircontrol -h` to see a quick overview over all available command line parameters.

The following **options** are available:

`-c <file>` &nbsp; Configuration file, defaulting to */etc/aircontrol.conf*.

`-d <file>` &nbsp; Specify an air scan dump file. Applicable only when air scanning (command parameter `-s`).

`-g <pin>` &nbsp; Override the GPIO pin to be used for scanning and targeting. The parameter must be a Broadcom GPIO number, not re-mapped. Might be used for quickly testing multiple transmitters or receivers.

`-l` &nbsp; Limit the number of aircontrol instances to 1, i.e. prevent multiple program instances.

The following **commands** are available, only one of them must be specified:

`-r <file>` &nbsp; Replay the given air scan dump file.

`-s <ms>` &nbsp; Perform an air scan for the given number of milliseconds. An ASCII graph will be written to stdout which can be redirected to a file with `tee` or something similar.

`-t <target>` &nbsp; Execute the given air target, i.e. transmit the target code as configured.

Either parameter `-r`, `-s` or `-t` is mandatory.


### **CONFIGURATION FILE**

The default configuration file is located in */etc/aircontrol.conf*. For details about the configuration file syntax check the manual of libconfig on its project site: <http://www.hyperrealm.com/libconfig/libconfig_manual.html>

The configuration consists of different sections explained below.

#### 'replay' section

This section defines parameters required for air replay.

`gpioPin` &nbsp; GPIO pin of the Raspberry Pi which is connected to the DATA line of a radio transmitter. This parameter expects Broadcom GPIO numbers, not re-mapped. Example: `gpioPin = 17;`

#### 'scan' section

This section defines all air scan relevant parameters.

`gpioPin` &nbsp; GPIO pin of the Raspberry Pi which is connected to the DATA line of a radio receiver. This parameter expects Broadcom GPIO numbers, not re-mapped. Example: `gpioPin = 18;`

`samplingRate` &nbsp; Delay between two samples when air scanning in microseconds. This parameter in combination with the `-s` value defines the number of segments being output. For example when scanning for 1ms (=1000us) with a `samplingRate` of 100us there will be 10 segments printed to stdout. Example: `samplingRate = 100;`

#### 'target' section

This section stores configuration defaults for all target sections.

`gpioPin` &nbsp; GPIO pin of the Raspberry Pi which is connected to the DATA line of a radio transmitter. This parameter expects Broadcom GPIO numbers, not re-mapped. Example: `gpioPin = 17;`

`dataLength` &nbsp; Pulse length of a single data element (0 or 1) in microseconds. Example: `dataLength = 1780;`

`syncLength` &nbsp; Pulse length of a single sync element (s or S) in microseconds. Example: `syncLength = 5000;`

`sendCommand` &nbsp; Number of times the air command will be transmitted. Example: `sendCommand = 10;`

`sendDelay` &nbsp; Delay between the air command transmissions in microseconds. Example: `sendDelay = 10000;`

`airCode` &nbsp; Encoding type of the air command. This parameter defines the validity and meaning of all `airCommand` values. The following radio frame encodings are currently supported. Example: `airCode = 0;`

                               _           _               _
    0  Manchester; values:  0)  |_    1) _|     s) _    S)
                                             _           ___
    1  Remote Controlled Outlet; values:  0)  |___    1)    |_
        (for reference: 00 -> 0, 11 -> 1, 01 -> F)
                     _          _   _
    2  Tormatic:  0)  |__    1)  |_| 


`airCommand` &nbsp; Sequence of values to be transmitted. The accepted values of this parameter are defined by airCode. Example: `airCode = 0; airCommand = "sss010011SSS";`

        _    __   _    _   ____
    ___| |__|  |_| |__| |_| 
    sss  0  1  0   0  1   1 SSS

#### Actual target sections

The actual target sections can be named freely, they incorporate all defaults from the 'target' section. All parameters from the 'target' section apply. For example all timing relevant parameters can be defined in the 'target' section while the real target sections only contain the appropriate `airCommand`.


### **AIR REPLAY**

aircontrol is able to record radio frames while scanning into a so called air scan dump file. This file can be used later to replay the recorded radio frames, see the following example:

Air scan for 1000ms and dump the results to `example.asd`:
> \# aircontrol -d example.asd -s 1000

Replay the previously recorded air scan dump `example.asd`:
> \# aircontrol -r example.asd
