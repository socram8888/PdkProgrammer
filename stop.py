#!/usr/bin/env python3

from usb1 import *
from libusb1 import *

CMD_END = 0
CMD_START = 1

PDK_ERASE = 3
PDK_WRITE = 7
PDK_READ = 6

ctx = USBContext()
dev = ctx.getByVendorIDAndProductID(0x16C0, 0x05DC)
handle = dev.open()

with handle.claimInterface(0):
    print(handle.getProduct())
    print(handle.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, CMD_END, 0, 0, 0))

handle.close()