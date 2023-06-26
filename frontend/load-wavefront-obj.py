import pywavefront
import socket
import sys
import struct

filename = None

for a in sys.argv[1:]:
    if ".obj" in a:
        filename = a

if filename is None:
    sys.exit(0)

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
        #print(material.has_faces)


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
        self.client6_.send(datas)

    def close(self):
        self.client6_.close()


tcp = Tcp()

tcp.connect("::1", 8080)

tcp.sendString(filename)

tcp.close()

