import pywavefront
import socket
import sys
import struct

###

class Tcp():
    def __init__(self):
        self.client6_ = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

    def connect(self, address, port):
        try:
            self.client6_.connect((address, port))
        except:
            pass

    def sendString(self, string):
        datas = bytearray()
        datas.extend(map(ord, string))
        datas.extend([0])
        #print(datas)
        self.client6_.send(datas)

    def sendInt(self, value):
        datas = struct.pack('<I', value)
        #print(datas)
        self.client6_.send(datas)

    def sendFloat(self, value):
        datas = struct.pack('<f', value)
        self.client6_.send(datas)

    def sendFloatArray(self, values):
        #print(len(values))
        self.sendInt(len(values))
        for value in values:
            self.sendFloat(value)

    def close(self):
        self.client6_.close()

###

filename = None

for a in sys.argv[1:]:
    if ".obj" in a:
        filename = a

if filename is None:
    sys.exit(0)

scene = None

try:
    scene = pywavefront.Wavefront(filename, create_materials=True, collect_faces=True)
    print("success to load %s" % filename)
    #print(scene.__dict__)
    for name, material in scene.materials.items():
        print(name)
        #print(material.vertex_format)
        #print(material.vertices)
        #print(material.diffuse)
        #print(material.ambient)
        #print(material.__dict__)

    for mesh in scene.mesh_list:
        #print(mesh.__dict__)
        for material in mesh.materials:
            print(material.vertex_format)
            pass
        for face in mesh.faces:
            pass        

except:
    print("failed to load %s" % filename)
    sys.exit(-1)

tcp = Tcp()

tcp.connect("::1", 8080)

tcp.sendString(filename)

if scene is None:
    pass
else:
    tcp.sendInt(len(scene.mesh_list))
    for mesh in scene.mesh_list:
        tcp.sendInt(len(mesh.materials))
        for material in mesh.materials:
            tcp.sendString(material.name)
            tcp.sendString(material.vertex_format)
            tcp.sendFloatArray(material.vertices)

tcp.close()

