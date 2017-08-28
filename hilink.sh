#!/bin/bash 

sudo rmmod xhci_hcd
sleep 2
sudo modprobe xhci_hcd
sleep 5
sudo modprobe cdc_ncm
sudo modprobe huawei_cdc_ncm
sleep 2

sudo modprobe cdc_ether
sudo modprobe usbnet
sudo modprobe usbcore
sudo modprobe option

sleep 5
#the following line works only for E3372 modems!
sudo usb_modeswitch -v 12d1 -p 1f01 -M '55534243123456780000000000000011062000000101000100000000000000' &
sleep 10

sudo ifconfig eth1 up
sudo ifconfig eth1 192.168.8.100 netmask 255.255.255.0
sudo route add default gw 192.168.8.1

