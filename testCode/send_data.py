import socket
import sys
import struct

# Create a UDS socket
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = '/tmp/power_data.sock'
print >>sys.stderr, 'connecting to %s' % server_address
try:
    sock.connect(server_address)
except socket.error, msg:
    print >>sys.stderr, msg
    sys.exit(1)

try:
    # Send data
    message = struct.pack("!HH", 1, 90)
    print >>sys.stderr, 'sending data'
    sock.sendall(message)

finally:
    print >>sys.stderr, 'closing socket'
    sock.close()
