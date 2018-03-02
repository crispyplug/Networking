#!/usr/bin/python

import time
import socket
import struct
import sys
import array

"""
	Source: https://docs.python.org/2/library/socket.html#example
	https://docs.python.org/2/library/struct.html

	NOTE: Ran code on tux209 with python version 2.6.6 
"""

# class that acts like a C struct
class RequestStruct():
	def __init__(self, tml, requestId, opCode, numOperands, op1, op2):
		self.tml = tml
		self.requestId = requestId
		self.opCode = opCode
		self.numOperands = numOperands
		self.op1 = op1
		self.op2 = op2

def GetUserInput(rStruct):
	opCheck = True
	firstOpCheck = True
	secondOpCheck = True

	# need to init these because I had to add a try except block in case a user doesn't enter anything
	userOp = 0
	firstUserOp = 0
	secondUserOp = 0

	while opCheck == True:
		try:
			userOp = int(raw_input("Please enter an opcode: "))
		except ValueError:
			print "\nIncorrect opcode.\n"
			continue
		
		if userOp >= 6 or userOp < 0:
			print "\nIncorrect opcode.\n"
		else:
			rStruct.opCode = userOp
			# correct opcode entered
			opCheck = False 

	while firstOpCheck == True:
		try:
			firstUserOp = int(raw_input("Please enter the first operand: "))
		except ValueError:
			print "\nPlease enter a valid operand (0-255 inclusive)\n"
			continue

		if firstUserOp < 0 or firstUserOp > 255:
			print "\nPlease enter a valid operand (0-255 inclusive)\n"
                else:
			# correct 1st op entered
			rStruct.op1 = firstUserOp
			rStruct.numOperands = 1
			firstOpCheck = False
	
	while secondOpCheck == True:
		try:
			secondUserOp = int(raw_input("Please enter the second operand: "))
		except ValueError:
			# Assuming the user doesn't want a second operand
			break

		if secondUserOp < 0 or secondUserOp > 255:
			print "\nPlease enter a valid operand (0-255 inclusive)\n"
		else:
			rStruct.op2 = secondUserOp
			rStruct.numOperands = rStruct.numOperands + 1
			secondOpCheck = False
	return rStruct

def ConnectSocket(servername, port):
	# keeping this consistent with original C code 
	ai_family = socket.AF_UNSPEC
	ai_socktype = socket.SOCK_STREAM
	sockfd = None

	try:
		servinfo = socket.getaddrinfo(servername, port, ai_family, ai_socktype)
	except socket.gaierror, msg: 
		print "\nUnable to connect to " + servername + "\n"
		return 1

	# loops through all the results and connects to the first one possible	
	for p in servinfo:
		ai_family, ai_socktype, ai_protocol, ai_cannonname, ai_sockaddr = p
		try:	
			sockfd = socket.socket(ai_family, ai_socktype, ai_protocol)
		except socket.error, msg:
			sockfd = None
			continue

		try:
			# connect function is slightly different in python
			sockfd.connect(ai_sockaddr)
		except socket.error, msg:
			sockfd.close()
			sockfd = None
			continue
		
		break

	if sockfd is None:
		sys.stderr.write("client: failed to connect\n")
		return 2

	return sockfd

def main(argv):
	# initalizes the struct (none of these values are actually strings but you have to init this way...)
	rStruct = RequestStruct("0", "0", "0", "0", "0", "0")

	if len(argv) != 3:
		sys.stderr.write("usage: client hostname\n")
		return 1

	rStruct = GetUserInput(rStruct)
	# just giving the requestId a random number that can be incremented easily
	rStruct.requestId = 1
	
	# init
	continueUserRequest = True
	sockfd = ConnectSocket(argv[1], argv[2])

	while continueUserRequest == True and sockfd is not None:
	
		start_time = time.time()

		rStruct.tml = 8
		packet = struct.pack('!cccchh', chr(rStruct.tml), chr(rStruct.requestId), chr(rStruct.opCode), chr(rStruct.numOperands), rStruct.op1, rStruct.op2)		
		sockfd.sendall(packet)
		data = sockfd.recv(1024)

		total_time = time.time() - start_time
		
		received_data = ""
		for i in data:
			received_data += str(hex(ord(i))) + " "
		print "\nclient received: " + received_data
		print "\nRound trip time: " + str(total_time) + " seconds"
		
		userInput = raw_input("\nWould you like to make another calculation (Y/N)? ")
		if userInput == 'Y' or userInput == 'y':
			rStruct = GetUserInput(rStruct)
			rStruct.requestId = int(rStruct.requestId) + 1
		else:
			continueUserRequest = False	
		sockfd.close()
		sockfd = ConnectSocket(argv[1], argv[2])

if __name__ == "__main__":
	sys.exit(main(sys.argv))
