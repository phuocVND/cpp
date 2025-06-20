import pybullet as p
import pybullet_data
import time

# Kết nối PyBullet GUI
p.connect(p.GUI)
p.setAdditionalSearchPath(pybullet_data.getDataPath())
p.setGravity(0, 0, -9.81)

robot = p.loadURDF("franka_panda/panda.urdf", useFixedBase=True)

controlled_joints = [0, 1, 2, 3, 4, 5]

# Lưu giới hạn khớp
joint_limits = {}
print("📌 Các khớp được điều khiển:")
for idx in controlled_joints:
    joint_info = p.getJointInfo(robot, idx)
    joint_name = joint_info[1].decode('utf-8')
    lower = joint_info[8]
    upper = joint_info[9]
    joint_limits[idx] = (lower, upper)
    print(f"Khớp {idx} ({joint_name}): {lower:.3f} → {upper:.3f} rad")

# 🎯 Target normalized [-1, 1]
normalized_targets = [0.2, -0.5, 0.4, 0.1, 0.0, -0.3]

# 🎯 Điểm đích trong không gian
goal_position = [0.5, 0.0, 0.5]  # x, y, z

# Hàm chuyển từ [-1, 1] → giá trị góc thực tế
def denormalize(norm_val, lower, upper):
    return (norm_val + 1) / 2 * (upper - lower) + lower

# Vẽ điểm đích (hình cầu xanh lá cây)
goal_visual_id = p.createVisualShape(
    shapeType=p.GEOM_SPHERE,
    radius=0.02,
    rgbaColor=[0, 1, 0, 1]
)
goal_marker = p.createMultiBody(
    baseVisualShapeIndex=goal_visual_id,
    basePosition=goal_position
)

# Mô phỏng
while True:
    # Điều khiển các khớp
    for i, joint_index in enumerate(controlled_joints):
        lower, upper = joint_limits[joint_index]
        target_angle = denormalize(normalized_targets[i], lower, upper)
        p.setJointMotorControl2(
            bodyIndex=robot,
            jointIndex=joint_index,
            controlMode=p.POSITION_CONTROL,
            targetPosition=target_angle,
            force=500
        )

    # Lấy vị trí đầu tay (link 11)
    ee_pos = p.getLinkState(robot, 11)[0]
    print(f"📍 Vị trí đầu tay: ({ee_pos[0]:.3f}, {ee_pos[1]:.3f}, {ee_pos[2]:.3f})")

    p.stepSimulation()
    time.sleep(1. / 240)
