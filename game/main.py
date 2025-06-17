import socket

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 8080
BUFFER_SIZE = 1024

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.bind((SERVER_HOST, SERVER_PORT))

server_socket.listen(5)
print(f"Server đang chạy tại {SERVER_HOST}:{SERVER_PORT}, đợi kết nối từ client...")

while True:
    client_socket, client_address = server_socket.accept()
    print(f"Đã kết nối với client: {client_address}")

    data = client_socket.recv(BUFFER_SIZE)
    print(data.decode())
    while True:
        name = input("chọn hướng\n")
        response = name

        try:
            client_socket.send(response.encode())
        except Exception as e:
            client_socket.close()

server_socket.close()
