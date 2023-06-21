import socket

client6 = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

client6.connect(("::1", 8080))

client6.send(b"shutdown")

client6.close()
