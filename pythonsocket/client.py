import socket

c = socket.socket()
print("connecting to server..")
name = input('Enter name')

c.connect(('localhost',8080))
print("connected!")
c.send(bytes(name,'utf-8'))
print(c.recv(1024).decode())

