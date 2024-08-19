import socket

s = socket.socket()
print('Socket created')

s.bind(('localhost',8080))

s.listen(3)
print('waiting...')

while True:
    c,addr = s.accept()
    name = c.recv(1024).decode()
    print('connected with client with addr and name ',addr,name)
    
    c.send(bytes('Welcome to the server','utf-8'))
    
    c.close()
    
    ō