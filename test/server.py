#!/usr/bin/env python
import socket
address=('', 12346)
s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
s.bind(address)
print("Start Waiting")
while 1:
	data, addr=s.recvfrom(2048)
	if not data:
		break
	print(data.decode())
	print(addr)
s.close()
