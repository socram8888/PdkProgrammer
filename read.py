#!/usr/bin/env python3

from usb1 import *
from libusb1 import *
import struct

CMD_START = 1

PDK_ERASE = 3
PDK_WRITE = 7
PDK_READ = 6

ctx = USBContext()
dev = ctx.getByVendorIDAndProductID(0x16C0, 0x05DC)
handle = dev.open()

with handle.claimInterface(0):
    print(handle.getProduct())
    devId, = struct.unpack("h", handle.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, CMD_START, PDK_READ, 0, 2))
    print('Device ID: %03X' % devId)

handle.close()