import socket
import threading

PORT = 5050
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER,PORT)
# print(SERVER)
server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server.bind(ADDR)


def handle_client(conn,addr):
   print(f"[New conn]" {addr} coneected.")
  connected = True
  while connected :
      msg = conn.recv(1024)
      

def start():
    server.listen()
    while True:
        conn,addr = server.accept()
        thread = threading.Thread(target=handle_client,args=(conn,addr))
        thread.start()
        print(f"[ACTIVE CONNCS] : {threading.activeCount() -1 }")

print("[STARTING] server is starting")
start()