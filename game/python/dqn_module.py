# dqn_module.py

import torch
import torch.nn as nn
import torch.optim as optim
import random
import numpy as np

class DQN(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(DQN, self).__init__()
        self.linear1 = nn.Linear(input_size, hidden_size)
        self.linear2 = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        x = torch.relu(self.linear1(x))
        return self.linear2(x)

def select_action(state, epsilon, model):
    if random.random() < epsilon:
        return random.randint(0, 3)
    state = torch.tensor(state, dtype=torch.float32)
    with torch.no_grad():
        q_values = model(state)
    return torch.argmax(q_values).item()

def train_step(model, target_model, memory, optimizer, gamma):
    if len(memory) < 100:
        return
    batch = random.sample(memory, 64)
    states, actions, rewards, next_states, dones = zip(*batch)

    states = torch.tensor(states, dtype=torch.float32)
    actions = torch.tensor(actions, dtype=torch.long)
    rewards = torch.tensor(rewards, dtype=torch.float32)
    next_states = torch.tensor(next_states, dtype=torch.float32)
    dones = torch.tensor(dones, dtype=torch.float32)

    q_values = model(states).gather(1, actions.unsqueeze(1)).squeeze()
    next_q_values = target_model(next_states).max(1)[0]
    targets = rewards + gamma * next_q_values * (1 - dones)

    loss = nn.functional.mse_loss(q_values, targets.detach())
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()
