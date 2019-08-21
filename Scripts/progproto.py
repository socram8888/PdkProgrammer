
from enum import Enum
from usb1 import *
from libusb1 import *
import array
import struct

class ProtoCommands(Enum):
	MODE = 0
	READ = 1
	WRITE = 2
	ERASE = 3

class ProtoModes(Enum):
	OFF = 0
	READ = 1
	WRITE = 2
	ERASE = 3

class Programmer:
	MAX_WORDS = 127

	def __init__(self, ctx_or_device=None):
		self._ctx = None
		self._mode = ProtoModes.OFF

		if ctx_or_device is None:
			self._ctx = USBContext()
			self._dev = self._open_programmer(self._ctx)

		elif isinstance(ctx_or_device, USBContext):
			self.usb_device = self._open_programmer(usb_context)

		elif isinstance(ctx_or_device, USBDevice):
			self._dev = usb_device.open()

		else:
			raise Exception("Invalid parameter passed")

	def close(self):
		self._dev.close()
		if self._ctx:
			self._ctx.close()

	def _open_programmer(self, ctx):
		for device in ctx.getDeviceIterator():
			if \
					device.getVendorID() == 0x16C0 and \
					device.getProductID() == 0x05DC and \
					device.getProduct() == "PFS154 prog v1":
				return device.open()

		raise Exception("No programmer found")

	def start_read(self, timeout=100):
		rx = self._dev.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, ProtoCommands.MODE, ProtoModes.READ, 0, 2, timeout)
		devId, = struct.unpack("<H", rx)

		if devId != 0xAA1:
			raise Exception("Unsupported device ID: %06X" % devId)

		self._mode = ProtoModes.READ

	def read(self, offset, length, timeout=500):
		if self._mode is not ProtoModes.READ:
			raise Exception("Not in read mode")

		words = array.array('H')
		while length > 0:
			iterlen = min(length, Programmer.MAX_WORDS)
			iterdata = self._dev.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, ProtoCommands.READ, 0, offset, iterlen * 2, timeout)

			if len(iterdata) != iterlen * 2:
				raise Exception("Expected to read %d bytes, got %d" % (iterlen * 2, len(iterdata)))

			words.frombytes(iterdata)
			offset += iterlen
			length -= iterlen

		return words

	def start_write(self, timeout=100):
		rx = self._dev.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, ProtoCommands.MODE, ProtoModes.WRITE, 0, 2, timeout)
		devId, = struct.unpack("<H", rx)

		if devId != 0xAA1:
			raise Exception("Unsupported device ID: %06X" % devId)

		self._mode = ProtoModes.WRITE

	def write(self, offset, words, timeout=500):
		if self._mode != ProtoModes.WRITE:
			raise Exception("Not in write mode")

		if len(words) % 4 != 0:
			raise Exception("Words must be programmed in chunks of 4")

		bytes = words.tobytes()
		while len(bytes) > 0:
			iterlen = min(len(bytes), Programmer.MAX_WORDS * 2)
			iterdata = bytes[0:iterlen]
			written = self._dev.controlWrite(LIBUSB_REQUEST_TYPE_VENDOR, ProtoCommands.WRITE, 0, offset, iterdata, timeout)

			if written != iterlen:
				raise Exception("Expected to write %d bytes, wrote %d" % (iterlen, written))

			bytes = bytes[iterlen:]

	def erase(self, timeout=500):
		rx = self._dev.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, ProtoCommands.MODE, ProtoModes.ERASE, 0, 2, timeout)
		devId, = struct.unpack("<H", rx)

		if devId != 0xAA1:
			raise Exception("Unsupported device ID: %06X" % devId)

		rx = self._dev.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, ProtoCommands.ERASE, 0, 0, 2, timeout)
		devIdErase, = struct.unpack("<H", rx)

		if devIdErase != devId:
			raise Exception("Erase failed (expected reply %06X, got %06X)" % (devId, devIdErase))

	def finish(self):
		if self._mode != ProtoModes.OFF:
			rx = self._dev.controlRead(LIBUSB_REQUEST_TYPE_VENDOR, ProtoCommands.MODE, ProtoModes.OFF, 0, 2)
			if rx != b"\x00\x00":
				raise Exception("Invalid response on finish")
			self._mode = ProtoModes.OFF

	def __enter__(self):
		return self

	def __exit__(self, exc_type, exc_val, exc_tb):
		self.close()
