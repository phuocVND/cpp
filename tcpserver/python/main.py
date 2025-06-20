import socket
import struct
import time
import threading
import pybullet as p
import pybullet_data

# ========================== PYBULLET SIMULATION ==========================
def run_simulation(get_target_func):
    p.connect(p.GUI)
    p.setAdditionalSearchPath(pybullet_data.getDataPath())
    p.setGravity(0, 0, -9.81)

    robot = p.loadURDF("franka_panda/panda.urdf", useFixedBase=True)
    controlled_joints = [0, 1, 2, 3, 4, 5]

    joint_limits = {}
    for idx in controlled_joints:
        joint_info = p.getJointInfo(robot, idx)
        lower = joint_info[8]
        upper = joint_info[9]
        joint_limits[idx] = (lower, upper)

    goal_position = [0.5, 0.0, 0.5]
    goal_visual_id = p.createVisualShape(p.GEOM_SPHERE, radius=0.02, rgbaColor=[0, 1, 0, 1])
    p.createMultiBody(baseVisualShapeIndex=goal_visual_id, basePosition=goal_position)

    def denormalize(norm_val, lower, upper):
        return (norm_val + 1) / 2 * (upper - lower) + lower

    while True:
        normalized_targets = get_target_func()
        if normalized_targets is None:
            continue

        for i, joint_index in enumerate(controlled_joints):
            lower, upper = joint_limits[joint_index]
            target_angle = denormalize(normalized_targets[i], lower, upper)
            p.setJointMotorControl2(robot, joint_index, p.POSITION_CONTROL, target_angle, force=500)

        ee_pos = p.getLinkState(robot, 11)[0]
        print(f"📍 Tay: ({ee_pos[0]:.3f}, {ee_pos[1]:.3f}, {ee_pos[2]:.3f})")

        p.stepSimulation()
        time.sleep(1. / 240)
# ========================================================================

# ========================== NETWORK CODE ==========================
SERVER_IP = '127.0.0.1'
SERVER_PORT = 12345

latest_array = [0, 0, 0, 0, 0, 0]  # Biến dùng chung giữa thread socket & PyBullet

def recv_array(sock, size):
    data = b''
    while len(data) < size * 2:
        packet = sock.recv(size * 2 - len(data))
        if not packet:
            return None
        data += packet
    return list(struct.unpack('!6h', data))  # 6 số short

def send_array(sock, arr):
    data = struct.pack('!6h', *arr)
    sock.sendall(data)

def normalized(val):
    return max(-1.0, min(1.0, (val - 180) / 180.0))

def socket_thread():
    global latest_array
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_sock:
        client_sock.connect((SERVER_IP, SERVER_PORT))

        welcome = client_sock.recv(1024).decode()
        print(f"📩 Server: {welcome}")

        client_sock.sendall(b"Hi server!")

        while True:
            arr = recv_array(client_sock, 6)
            if arr is None:
                print("❌ Server ngắt kết nối")
                break

            print(f"✅ Nhận: {arr}")
            latest_array = [normalized(x) for x in arr]

            # Gửi xác nhận
            send_array(client_sock, arr)
            time.sleep(0.01)

def get_current_target():
    global latest_array
    return latest_array
# ========================================================================

if __name__ == "__main__":
    # Chạy thread nhận dữ liệu socket
    t = threading.Thread(target=socket_thread, daemon=True)
    t.start()

    # Chạy mô phỏng robot sử dụng dữ liệu mới nhất
    run_simulation(get_current_target)
