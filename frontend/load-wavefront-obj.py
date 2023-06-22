import pywavefront
import socket
import sys

filename = None

for a in sys.argv[1:]:
    if ".obj" in a:
        filename = a

if filename is None:
    sys.exit(0)

try:
    meshes = pywavefront.Wavefront(filename)
    print("success to load %s" % filename)
except:
    print("failed to load %s" % filename)
    sys.exit(-1)

client6 = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

client6.connect(("::1", 8080))

client6.send(b"Hello")

client6.close()
