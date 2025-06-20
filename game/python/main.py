from asyncio import sleep
from collections import deque
import os
import socket
import struct
import threading
import time
from dqn_module import DQN, select_action, train_step
import torch.optim as optim
import numpy as np
import torch
MODEL_PATH = "dqn_model.pth"

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 8080
BUFFER_SIZE = 8
WIDTH, HEIGHT = 200.0, 200.0  # Dùng để chuẩn hóa tọa độ

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print("🔥 Using device:", device)

print("CUDA available:", torch.cuda.is_available())
# print("Current device:", torch.cuda.current_device())
# print("Device name:", torch.cuda.get_device_name(torch.cuda.current_device()))

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
    xHeadSnack = 0
    yHeadSnack = 0
    xFood = 0
    yFood = 0
    done = 0
    lastAction = 3;
    learningrate = 0.0001
    gamma = 0.9
    epsilon = 1.0
    epsilon_decay = 1.0 - (1.0/100000)
    min_epsilon = 0.01
    memory = deque(maxlen=10000)

    model = DQN(6, 128, 4).to(device)
    target_model = DQN(6, 128, 4).to(device)

    if os.path.exists(MODEL_PATH):
        checkpoint = torch.load(MODEL_PATH, weights_only=False, map_location=device)
        model.load_state_dict(checkpoint['model'])
        epsilon = checkpoint.get('epsilon', 1.0)
        memory = checkpoint.get('memory', [])
        print("✅ Đã load model từ file.")
    else:
        print("⚠️ Không tìm thấy file model, khởi tạo model mới.")

    target_model.load_state_dict(model.state_dict())
    optimizer = optim.Adam(model.parameters(), learningrate)


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
                torch.save({
                    'model': model.state_dict(),
                    'epsilon': epsilon,
                    'memory': memory
                }, MODEL_PATH)
                            
                sizeData = recvValue(client_socket) #nhận size
                data = client_socket.recv(sizeData)
                if not data:
                    break
                # print(f"📥 Nhận từ client: {data.decode()}")

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
                lastAction = int(parts[5])
                # print(f"[Client] gửi: {xHeadSnack, yHeadSnack, xFood, yFood, done, lastAction}")
                dir_x, dir_y = 0,0;
                # chuyển thành vector hướng:
                if lastAction == 1:
                    dir_x, dir_y = 0, -1  # up
                elif lastAction == 2:
                    dir_x, dir_y = 0, 1   # down
                elif lastAction == 3:
                    dir_x, dir_y = 1, 0  # right
                elif lastAction == 4:
                    dir_x, dir_y = -1, 0   # left

                state = np.array([
                    xHeadSnack / WIDTH,
                    yHeadSnack / HEIGHT,
                    xFood / WIDTH,
                    yFood / HEIGHT,
                    dir_x,
                    dir_y
                ])

                action = select_action(state, epsilon, model)
                directions = ['u', 'd', 'r', 'l']
                action_str = directions[action]
                # print(f"🤖 AI chọn: {action_str}")

                while True:
                    # user_input = input("Chọn hướng:\n").strip()
                    ai_input = action_str
                    if ai_input:
                        break
                    # print("Không được để trống! Vui lòng nhập lại.")
                try:
                    sendValue(client_socket, len(ai_input))
                    dataSend = ai_input.encode('utf-8')
                    client_socket.sendall(dataSend)
                except Exception as e:
                    # print(f"Lỗi khi gửi dữ liệu: {e}")
                    client_socket.close()
                    break
                
                # Tạo reward
                if (lastAction == 1 and action == 1) or \
                (lastAction == 2 and action == 0) or \
                (lastAction == 3 and action == 3) or \
                (lastAction == 4 and action == 2):
                    reward = -5.0
                elif    xHeadSnack < 0 or yHeadSnack < 0 or \
                        xHeadSnack >= WIDTH or yHeadSnack >= HEIGHT:
                    reward = -5.0  # Va chạm tường
                else:
                    reward = 10.0 if done else -0.1
                next_state = state  # đơn giản hóa

                memory.append((state, action, reward, next_state, float(done)))
                train_step(model, target_model, memory, optimizer, gamma)

                # Giảm epsilon
                epsilon = max(min_epsilon, epsilon * epsilon_decay)
                
    except KeyboardInterrupt:
        print("\nServer đã dừng.")
    finally:
        try:
            torch.save({
                'model': model.state_dict(),
                'epsilon': epsilon,
                'memory': memory
            }, MODEL_PATH)
            print("💾 Đã lưu model vào file.")
        except Exception as e:
            print(f"❌ Lỗi khi lưu model: {e}")
        finally:
            server_socket.close()
            print("🔌 Server đã đóng kết nối.")


if __name__ == "__main__":
    main()
