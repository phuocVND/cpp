from asyncio import sleep
import socket
import struct
import threading
import time
from dqn_module import DQN, select_action, train_step
import torch.optim as optim

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 8080
BUFFER_SIZE = 8

xHeadSnack = 0
yHeadSnack = 0
xFood = 0
yFood = 0
done = 0

model = DQN(6, 128, 4)
target_model = DQN(6, 128, 4)
target_model.load_state_dict(model.state_dict())
optimizer = optim.Adam(model.parameters(), lr=0.001)

gamma = 0.9
epsilon = 1.0
epsilon_decay = 0.995
min_epsilon = 0.01
memory = []


def recv_all(sock, length):
    """Nhận chính xác `length` byte từ socket."""
    data = b''
    while len(data) < length:
        more = sock.recv(length - len(data))
        if not more:
            raise ConnectionError("Kết nối bị đóng trước khi nhận đủ dữ liệu.")
        data += more
    return data
def recvValue(client_socket):
    sizeData = recv_all(client_socket, BUFFER_SIZE) #nhận size
    sizeData = struct.unpack('!Q', sizeData)[0]
    return sizeData
def sendValue(client_socket, sizeData):
    data = struct.pack("!Q", sizeData)
    client_socket.sendall(data)
def main():

    # Tạo socket TCP
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_HOST, SERVER_PORT))
    server_socket.listen(5)

    print(f"Server đang chạy tại {SERVER_HOST}:{SERVER_PORT}, đợi kết nối từ client...")

    try:
        while True:
            client_socket, client_address = server_socket.accept()

            print(f"Đã kết nối với client: {client_address}")

            while True:
 
                sizeData = recvValue(client_socket) #nhận size
                data = client_socket.recv(sizeData)
                if not data:
                    break
                print(f"📥 Nhận từ client: {data.decode()}")

                dataSend = b"Phan hoi tu server"
                sendValue(client_socket, len(dataSend))
                client_socket.sendall(dataSend)

                sizeData = recvValue(client_socket) #nhận size  
                data = client_socket.recv(sizeData).decode().strip()
                if not data:
                    break
                parts = data.split(',')
                xHeadSnack = float(parts[0])
                yHeadSnack = float(parts[1])
                xFood = float(parts[2])
                yFood = float(parts[3])
                done = parts[4] == '1'

                print(f"[Client] gửi: {xHeadSnack, yHeadSnack, xFood, yFood, done}")

                while True:
                    user_input = input("Chọn hướng:\n").strip()
                    if user_input:
                        break
                    print("Không được để trống! Vui lòng nhập lại.")
                try:
                    sendValue(client_socket, len(user_input))
                    dataSend = user_input.encode('utf-8')
                    client_socket.sendall(dataSend)
                except Exception as e:
                    print(f"Lỗi khi gửi dữ liệu: {e}")
                    client_socket.close()
                    break
                # time.sleep(1)
    except KeyboardInterrupt:
        print("\nServer đã dừng.")
    finally:
        server_socket.close()


if __name__ == "__main__":
    main()
