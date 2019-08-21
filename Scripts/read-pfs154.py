
from progproto import *

with Programmer() as p:
	try:
		p.start_read()
		print(p.read(0, 2048).tobytes().hex())
	finally:
		p.finish()
