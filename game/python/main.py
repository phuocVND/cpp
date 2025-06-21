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
TARGET_UPDATE_FREQUENCY = 10 # Ví dụ: cập nhật sau mỗi 10 ván game
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
    epsilon_decay = 1.0 - (1.0/50000)
    min_epsilon = 0.01
    memory = deque(maxlen=100000)

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

    print(epsilon)
    # Tạo socket TCP
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_HOST, SERVER_PORT))
    server_socket.listen(5)

    print(f"Server đang chạy tại {SERVER_HOST}:{SERVER_PORT}, đợi kết nối từ client...")

    try:
        while True:
            client_socket, client_address = server_socket.accept()

            print(f"Đã kết nối với client: {client_address}")
            last_state = None
            last_action = None
            games_played = 0
            while True:
                # torch.save({
                #     'model': model.state_dict(),
                #     'epsilon': epsilon,
                #     'memory': memory
                # }, MODEL_PATH)
                            
                sizeData = recvValue(client_socket) #nhận size  
                data = client_socket.recv(sizeData).decode().strip()
                if not data:
                    break
                parts = data.split(',')
                xHeadSnack = float(parts[0])
                yHeadSnack = float(parts[1])
                xFood = float(parts[2])
                yFood = float(parts[3])
                done = parts[4] == '1' # là cờ ăn mồi
                lastAction = int(parts[5]) #là hướng đi
                checkDead = parts[6] == '1' # là cờ báo thua
                # print(f"[Client] gửi: {xHeadSnack, yHeadSnack, xFood, yFood, done, lastAction}")
                dir_x, dir_y = 0,0;

                # chuyển thành vector hướng:
                if lastAction == 0:
                    dir_x, dir_y = 0, -1  # up
                elif lastAction == 1:
                    dir_x, dir_y = 0, 1   # down
                elif lastAction == 2:
                    dir_x, dir_y = 1, 0  # right
                elif lastAction == 3:
                    dir_x, dir_y = -1, 0   # left

                current_state = np.array([
                    xHeadSnack / WIDTH,
                    yHeadSnack / HEIGHT,
                    xFood / WIDTH,
                    yFood / HEIGHT,
                    dir_x,
                    dir_y
                ])

                if last_state is not None:
                    if(checkDead): 
                        reward = -100.0;
                    elif done:
                        reward = 50.0
                    else: 
                        reward = -0.1
                    
                    #+ 2. Lưu kinh nghiệm hoàn chỉnh vào bộ nhớ
                    #+    (state_cũ, action_cũ, reward_mới, state_mới, cờ_done_mới)
                    memory.append((last_state, last_action, reward, current_state, float(checkDead)))

                    #+ 3. Huấn luyện model
                    train_step(model, target_model, memory, optimizer, gamma)

                action = select_action(current_state, epsilon, model)
                directions = ['u', 'd', 'r', 'l']
                action_str = directions[action]
                # print(f"🤖 AI chọn: {action_str}")
                try:
                    sendValue(client_socket, len(action_str))
                    dataSend = action_str.encode('utf-8')
                    client_socket.sendall(dataSend)
                except Exception as e:
                    client_socket.close()
                    break




                #+ Lưu lại trạng thái và hành động hiện tại để dùng cho vòng lặp tiếp theo
                last_state = current_state
                last_action = action

                # #+ Xử lý khi game kết thúc
                # if done:
                #     print("Game Over. Chuẩn bị ván mới.")

                if checkDead: # Chỉ xử lý khi thực sự thua
                    games_played += 1
                    # print(f"Game Over. Ván thứ {games_played} đã kết thúc. Chuẩn bị ván mới.")

                    # Cập nhật target network sau mỗi TARGET_UPDATE_FREQUENCY ván
                    if games_played % TARGET_UPDATE_FREQUENCY == 0:
                        print(f"🔄 Cập nhật Target Network sau {games_played} ván.")
                        target_model.load_state_dict(model.state_dict())

                    # Reset lại để không học từ kinh nghiệm "vượt game"
                    last_state = None
                    last_action = None
                    # epsilon += (1.0/50000)
                #+ Giảm epsilon sau mỗi bước đi
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
