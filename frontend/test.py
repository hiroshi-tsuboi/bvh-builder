import socket

client6 = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

client6.connect(("::1", 8080))

client6.send(b"Hello!")

client6.close()

# shutdown

client4 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client4.connect(("127.0.0.1", 8080))

client4.send(b"shutdown")

client4.close()
