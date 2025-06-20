# robot_sim.py
import pybullet as p
import pybullet_data
import time

# Toàn bộ đoạn mã bạn đang có...

def run_simulation(normalized_targets):
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
        for i, joint_index in enumerate(controlled_joints):
            lower, upper = joint_limits[joint_index]
            target_angle = denormalize(normalized_targets[i], lower, upper)
            p.setJointMotorControl2(robot, joint_index, p.POSITION_CONTROL, target_angle, force=500)

        ee_pos = p.getLinkState(robot, 11)[0]
        print(f"📍 Vị trí đầu tay: ({ee_pos[0]:.3f}, {ee_pos[1]:.3f}, {ee_pos[2]:.3f})")

        p.stepSimulation()
        time.sleep(1. / 240)
