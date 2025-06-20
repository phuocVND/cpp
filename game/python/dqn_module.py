# dqn_module.py

import torch
import torch.nn as nn
import torch.optim as optim
import random
import numpy as np

# Thiết bị (CPU hoặc GPU)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

class DQN(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(DQN, self).__init__()
        self.linear1 = nn.Linear(input_size, hidden_size)
        self.linear2 = nn.Linear(hidden_size, hidden_size)
        self.linear3 = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        x = torch.relu(self.linear1(x))
        x = torch.relu(self.linear2(x))
        return self.linear3(x)

def select_action(state, epsilon, model):
    if random.random() < epsilon:
        return random.randint(0, 3)

    state = torch.tensor(state, dtype=torch.float32).unsqueeze(0).to(device)  # [1, 6]
    with torch.no_grad():
        q_values = model(state)
    return torch.argmax(q_values).item()

def train_step(model, target_model, memory, optimizer, gamma):
    if len(memory) < 256:
        return
    batch = random.sample(memory, 256)
    states, actions, rewards, next_states, dones = zip(*batch)

    # Chuyển thành tensor và đưa lên GPU
    states = torch.tensor(states, dtype=torch.float32).to(device)
    actions = torch.tensor(actions, dtype=torch.long).to(device)
    rewards = torch.tensor(rewards, dtype=torch.float32).to(device)
    next_states = torch.tensor(next_states, dtype=torch.float32).to(device)
    dones = torch.tensor(dones, dtype=torch.float32).to(device)

    # Q-values cho hành động đã chọn
    q_values = model(states).gather(1, actions.unsqueeze(1)).squeeze()

    next_actions = model(next_states).max(1)[1]
    next_q_values = target_model(next_states).gather(1, next_actions.unsqueeze(1)).squeeze()
    targets = rewards + gamma * next_q_values * (1 - dones)

    # Tính loss và cập nhật
    loss = nn.functional.smooth_l1_loss(q_values, targets.detach())
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()
