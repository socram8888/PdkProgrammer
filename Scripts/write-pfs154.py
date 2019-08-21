
from progproto import *
from array import array

data = array('H', [0] * 16)

with Programmer() as p:
	try:
		p.start_write()
		for i in range(4, 2048, 16):
			p.write(i, data)
	finally:
		p.finish()
