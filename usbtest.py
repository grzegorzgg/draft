#!/usr/bin/python
#
#Simple example on how to send and receive data to the Mbed over USB (on Linux) using pyUSB 1.0
#
import os
import sys
 
import usb.core
import usb.util
 
from struct import Struct
from sys import version_info as sys_version
from time import sleep
import random


ps4raport = {}

class StructHack(Struct):
    """Python <2.7.4 doesn't support struct unpack from bytearray."""
    def unpack_from(self, buf, offset=0):
        buf = buffer(buf)

        return Struct.unpack_from(self, buf, offset)


if sys_version[0] == 2 and sys_version[1] <= 7 and sys_version[2] <= 4:
    S16LE = StructHack("<h")
else:
    S16LE = Struct("<h")


 
# handler called when a report is received
def rx_handler(data):

    # clear = lambda: os.system('clear')
    # clear()

    ps4raport['analog'] = {}
    ps4raport['analog']['L2'] = data[8]
    ps4raport['analog']['R2'] = data[9]

    ps4raport['joystick'] = {}
    ps4raport['joystick']['L3'] = {'x': data[1], 'y': data[2]}
    ps4raport['joystick']['R3'] = {'x': data[3], 'y': data[4]}

    

    ps4raport['buttons'] = {}
    ps4raport['buttons']['share'] = (data[6] & 16) != 0
    ps4raport['buttons']['option'] = (data[6] & 32) != 0
    
    ps4raport['buttons']['cross'] = (data[5] & 32) != 0
    ps4raport['buttons']['circle'] = (data[5] & 64) != 0
    ps4raport['buttons']['square'] = (data[5] & 16) != 0
    ps4raport['buttons']['triangle'] = (data[5] & 128) != 0

    ps4raport['acceleration'] = {'1':S16LE.unpack_from(data, 13)[0], '2':S16LE.unpack_from(data, 15)[0], '3':S16LE.unpack_from(data, 17)[0]}






    # ggval[0] = data[1]
    # ggval[1] = data[2]

    # ggval[2] = data[3]
    # ggval[3] = data[4]

    # print '', data
    # print 'share\t', (data[6] & 16) != 0, 'option\t', (data[6] & 32) != 0, 'cross\t', (data[5] & 32) != 0, 'circle\t', ((data[5] & 64) != 0), 'square\t', (data[5] & 16) != 0, 'triangle\t', (data[5] & 128) != 0
    
    # print 'L1 \t', (data[6] & 1) != 0, 'L2 \t', (data[6] & 4) != 0
    # print 'Acceleration \t', S16LE.unpack_from(data, 13)[0], ' :: ', S16LE.unpack_from(data, 15)[0], ' :: ', S16LE.unpack_from(data, 17)[0]
    # print ':: ',  data[1], ' :: ', data[2]

 
def findHIDDevice(mbed_vendor_id, mbed_product_id):
    # Find device
    hid_device = usb.core.find(idVendor=mbed_vendor_id,idProduct=mbed_product_id)
    
    if not hid_device:
        print "No device connected"
    else:
        sys.stdout.write('mbed found\n')
        if hid_device.is_kernel_driver_active(0):
            try:
                hid_device.detach_kernel_driver(0)
            except usb.core.USBError as e:
                sys.exit("Could not detatch kernel driver: %s" % str(e))
        try:
            hid_device.set_configuration()
            hid_device.reset()
        except usb.core.USBError as e:
            sys.exit("Could not set configuration: %s" % str(e))
        
        endpoint = hid_device[0][(0,0)][0]      

        while True:
            data = [0x0] * 16

            #read the data
            bytes = hid_device.read(endpoint.bEndpointAddress, 64)
            rx_handler(bytes);

            print ps4raport
            # print ps4raport['buttons']
            # print ps4raport['acceleration']

            data = bytearray(32)
            data[0] = 05
            data[1] = 255

            data[4] = ps4raport['analog']['R2']
            data[5] = ps4raport['analog']['L2']

            data[6] = ps4raport['analog']['L2']
            data[7] = ps4raport['joystick']['L3']['x']
            data[8] = ps4raport['analog']['R2']
            
            data[9] = 0
            data[10] = 0
            
            hid_device.write(3, data)

if __name__ == '__main__':
    # The vendor ID and product ID used in the Mbed program
    mbed_vendor_id = 0x1234 
    mbed_product_id = 0x0006
 
    # Search the Mbed, attach rx handler and send data
    # findHIDDevice(mbed_vendor_id, mbed_product_id)
    findHIDDevice(0x054c, 0x05c4)