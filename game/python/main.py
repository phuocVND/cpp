import socket
from dqn_module import DQN, select_action, train_step
import torch.optim as optim

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 8080
BUFFER_SIZE = 1024

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


def handle_client(client_socket, client_address):
    print(f"Đã kết nối với client: {client_address}")

    # Nhận dữ liệu đầu tiên từ client
    initial_data = client_socket.recv(BUFFER_SIZE)
    print(initial_data.decode())
    client_socket.send("Hello from server!\n".encode())

    while True:
        data = client_socket.recv(BUFFER_SIZE).decode().strip()
        if not data:
            break
        # print(f"[Client] gửi: {data}")
        parts = data.split(',')
        xHeadSnack = float(parts[0])
        yHeadSnack = float(parts[1])
        xFood = float(parts[2])
        yFood = float(parts[3])
        done = parts[4] == '1'
        print(f"[Client] gửi: {xHeadSnack, yHeadSnack, xFood, yFood, done}")

        # Nhập phản hồi từ người dùng
        while True:
            user_input = input("Chọn hướng:\n").strip()
            if user_input:
                break
            print("Không được để trống! Vui lòng nhập lại.")

        try:
            client_socket.send(user_input.encode())
        except Exception as e:
            print(f"Lỗi khi gửi dữ liệu: {e}")
            client_socket.close()
            break


def main():




    # Tạo socket TCP
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_HOST, SERVER_PORT))
    server_socket.listen(5)

    print(f"Server đang chạy tại {SERVER_HOST}:{SERVER_PORT}, đợi kết nối từ client...")

    try:
        while True:
            client_socket, client_address = server_socket.accept()
            handle_client(client_socket, client_address)
    except KeyboardInterrupt:
        print("\nServer đã dừng.")
    finally:
        server_socket.close()


if __name__ == "__main__":
    main()
