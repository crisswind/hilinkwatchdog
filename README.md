# Hilink watchdog

Watchdog for LTE modems with hilink

Hilink watchdog is a simple application for monitoring LTE modems under linux operating system.
The app provides following features:
  - read session variables
  - get modem status
  - get LTE connection status
  - soft-reset modem
  - hard-reset modem (based on kernel modules reloading)
  - reboot if specified number of hard-resets had not brought modem online

This is the first version, without configuration files, just a simple application and bash script.


### Installation

Hilink watchdog requires linux operating system with pthreads, curl, usb modeswitch and modern gcc compiler.

```sh
$ cp hilink.sh /bin/hilink.sh
$ make
$ ./hilinkwatchdog
```

Remember to set correct identifiers for your modem in hilink.sh (usb_modeswitch params) and in main.cpp (lsusb arguments)
defines:
#define _DEBUG_

#define _MAX_RETRY_CT_ 5

#define _LOOP_TIMEOUT_ 45

Hilink interface name (currently eth1, address: 192.168.8.100 - default for Huawei hilink modems is 192.168.8.1)


### Configuration

Currently there's no additional configuration files or anything like that.
If anyone will be interested in using this app for modems other than Huawei E3372, then I'll consider adding *.ini files support and moving all configuration parameters into config files.


### Running

Just add to your rc.local:

/bin/hilink.sh &

screen -dmS watchdog /bin/start_watchdog.sh &  (or any other way to start the daemon in background)

That's it!
