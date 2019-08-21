
from progproto import *

with Programmer() as p:
	try:
		p.erase()
	finally:
		p.finish()
