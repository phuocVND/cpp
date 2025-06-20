import pygame
import random
import numpy as np
from collections import deque

# --- Phần 1: Thiết lập môi trường game (SnakeGame Class) ---

# Các hằng số
SCREEN_WIDTH = 600
SCREEN_HEIGHT = 480
GRID_SIZE = 20
GRID_WIDTH = SCREEN_WIDTH // GRID_SIZE
GRID_HEIGHT = SCREEN_HEIGHT // GRID_SIZE

# Màu sắc
WHITE = (255, 255, 255)
RED = (200, 0, 0)
GREEN1 = (0, 255, 0)
GREEN2 = (0, 200, 0)
BLACK = (0, 0, 0)

# Hướng
UP = (0, -1)
DOWN = (0, 1)
LEFT = (-1, 0)
RIGHT = (1, 0)

class SnakeGame:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption('Snake RL')
        self.clock = pygame.time.Clock()
        self.reset()

    def reset(self):
        self.direction = random.choice([UP, DOWN, LEFT, RIGHT])
        self.head = (GRID_WIDTH // 2, GRID_HEIGHT // 2)
        self.snake = deque([self.head, 
                            (self.head[0] - self.direction[0], self.head[1] - self.direction[1]),
                            (self.head[0] - 2 * self.direction[0], self.head[1] - 2 * self.direction[1])])
        self.score = 0
        self.food = None
        self._place_food()
        self.frame_iteration = 0

    def _place_food(self):
        while True:
            x = random.randint(0, GRID_WIDTH - 1)
            y = random.randint(0, GRID_HEIGHT - 1)
            self.food = (x, y)
            if self.food not in self.snake:
                break

    def play_step(self, action):
        self.frame_iteration += 1
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()

        # 1. Cập nhật hướng đi dựa trên action
        self._move(action)
        
        # 2. Di chuyển đầu rắn
        self.snake.appendleft(self.head)

        # 3. Kiểm tra các sự kiện trong game
        reward = 0
        game_over = False
        
        # Game over nếu đi quá lâu không ăn mồi
        if self.frame_iteration > 100 * len(self.snake):
            game_over = True
            reward = -100 # Phạt nặng
            return reward, game_over, self.score

        # Kiểm tra va chạm
        if self._is_collision():
            game_over = True
            reward = -100 # Phạt nặng
            return reward, game_over, self.score

        # Kiểm tra ăn mồi
        if self.head == self.food:
            self.score += 1
            reward = 10 # Thưởng
            self._place_food()
        else:
            self.snake.pop()

        self._update_ui()
        self.clock.tick(60) # Tốc độ game
        return reward, game_over, self.score

    def _is_collision(self, pt=None):
        if pt is None:
            pt = self.head
        # Va chạm tường
        if pt[0] >= GRID_WIDTH or pt[0] < 0 or pt[1] >= GRID_HEIGHT or pt[1] < 0:
            return True
        # Va chạm thân
        if pt in list(self.snake)[1:]:
            return True
        return False

    def _update_ui(self):
        self.screen.fill(BLACK)
        for i, segment in enumerate(self.snake):
            color = GREEN2 if i == 0 else GREEN1
            pygame.draw.rect(self.screen, color, (segment[0]*GRID_SIZE, segment[1]*GRID_SIZE, GRID_SIZE, GRID_SIZE))
        pygame.draw.rect(self.screen, RED, (self.food[0]*GRID_SIZE, self.food[1]*GRID_SIZE, GRID_SIZE, GRID_SIZE))
        pygame.display.flip()

    def _move(self, action):
        # action = [thẳng, rẽ phải, rẽ trái]
        # ví dụ: [1, 0, 0] -> đi thẳng
        
        clock_wise = [RIGHT, DOWN, LEFT, UP]
        idx = clock_wise.index(self.direction)

        if np.array_equal(action, [1, 0, 0]):
            new_dir = self.direction # Đi thẳng
        elif np.array_equal(action, [0, 1, 0]):
            next_idx = (idx + 1) % 4
            new_dir = clock_wise[next_idx] # Rẽ phải
        else: # [0, 0, 1]
            next_idx = (idx - 1) % 4
            new_dir = clock_wise[next_idx] # Rẽ trái
        
        self.direction = new_dir
        
        x = self.head[0] + self.direction[0]
        y = self.head[1] + self.direction[1]
        self.head = (x, y)


# --- Phần 2: Tác nhân AI (Agent Class) ---

MAX_MEMORY = 100_000
BATCH_SIZE = 1000
LR = 0.001 # Learning Rate

class Agent:
    def __init__(self):
        self.n_games = 0
        self.epsilon = 0  # Tham số kiểm soát sự ngẫu nhiên (exploration)
        self.gamma = 0.9  # Discount rate
        self.memory = deque(maxlen=MAX_MEMORY)
        # Q-table: Dùng dictionary để lưu, key là state, value là array 3 phần tử cho 3 actions
        self.q_table = {} 

    def get_state(self, game):
        head = game.head
        point_l = (head[0] - 1, head[1])
        point_r = (head[0] + 1, head[1])
        point_u = (head[0], head[1] - 1)
        point_d = (head[0], head[1] + 1)

        dir_l = game.direction == LEFT
        dir_r = game.direction == RIGHT
        dir_u = game.direction == UP
        dir_d = game.direction == DOWN

        state = [
            # Mối nguy hiểm ở phía trước
            (dir_r and game._is_collision(point_r)) or
            (dir_l and game._is_collision(point_l)) or
            (dir_u and game._is_collision(point_u)) or
            (dir_d and game._is_collision(point_d)),

            # Mối nguy hiểm ở bên phải
            (dir_u and game._is_collision(point_r)) or
            (dir_d and game._is_collision(point_l)) or
            (dir_l and game._is_collision(point_u)) or
            (dir_r and game._is_collision(point_d)),

            # Mối nguy hiểm ở bên trái
            (dir_d and game._is_collision(point_r)) or
            (dir_u and game._is_collision(point_l)) or
            (dir_r and game._is_collision(point_u)) or
            (dir_l and game._is_collision(point_d)),

            # Hướng di chuyển
            dir_l,
            dir_r,
            dir_u,
            dir_d,

            # Vị trí mồi
            game.food[0] < game.head[0],  # Mồi ở bên trái
            game.food[0] > game.head[0],  # Mồi ở bên phải
            game.food[1] < game.head[1],  # Mồi ở phía trên
            game.food[1] > game.head[1]   # Mồi ở phía dưới
        ]
        return tuple(state) # Dùng tuple làm key cho dictionary

    def remember(self, state, action, reward, next_state, done):
        self.memory.append((state, action, reward, next_state, done))

    def train_long_memory(self):
        if len(self.memory) > BATCH_SIZE:
            mini_sample = random.sample(self.memory, BATCH_SIZE)
        else:
            mini_sample = self.memory
        
        for state, action, reward, next_state, done in mini_sample:
            self.train_short_memory(state, action, reward, next_state, done)

    def train_short_memory(self, state, action, reward, next_state, done):
        # Lấy Q-values hiện tại cho state
        if state not in self.q_table:
            self.q_table[state] = np.zeros(3)
        
        # Lấy Q-values cho next_state
        if next_state not in self.q_table:
            self.q_table[next_state] = np.zeros(3)

        target = reward
        if not done:
            target = reward + self.gamma * np.max(self.q_table[next_state])

        # Cập nhật Q-value cho action đã chọn
        action_idx = np.argmax(action)
        q_value = self.q_table[state][action_idx]
        new_q = q_value + LR * (target - q_value)
        
        self.q_table[state][action_idx] = new_q


    def get_action(self, state):
        # Exploration vs Exploitation
        # Càng chơi nhiều, càng ít ngẫu nhiên
        self.epsilon = 80 - self.n_games
        final_move = [0, 0, 0]
        if random.randint(0, 200) < self.epsilon:
            move = random.randint(0, 2)
            final_move[move] = 1
        else:
            if state not in self.q_table:
                 self.q_table[state] = np.zeros(3)
            
            prediction = self.q_table[state]
            move = np.argmax(prediction)
            final_move[move] = 1
        
        return final_move

# --- Phần 3: Vòng lặp huấn luyện ---

def train():
    scores = []
    mean_scores = []
    total_score = 0
    record = 0
    agent = Agent()
    game = SnakeGame()

    while True:
        # 1. Lấy trạng thái hiện tại
        state_old = agent.get_state(game)

        # 2. Đưa ra hành động
        final_move = agent.get_action(state_old)

        # 3. Thực hiện hành động và nhận thông tin mới
        reward, done, score = game.play_step(final_move)
        state_new = agent.get_state(game)

        # 4. Huấn luyện bộ nhớ ngắn (cho mỗi bước)
        agent.train_short_memory(state_old, final_move, reward, state_new, done)

        # 5. Lưu vào bộ nhớ dài
        agent.remember(state_old, final_move, reward, state_new, done)

        if done:
            # Khi kết thúc game, huấn luyện lại từ bộ nhớ dài (experience replay)
            game.reset()
            agent.n_games += 1
            agent.train_long_memory()

            if score > record:
                record = score
                # Lưu model (Q-table) khi có kỷ lục mới
                # pickle.dump(agent.q_table, open('q_table.pkl', 'wb'))

            print(f'Game: {agent.n_games}, Score: {score}, Record: {record}')

            # Vẽ biểu đồ điểm số (tùy chọn)
            scores.append(score)
            total_score += score
            mean_score = total_score / agent.n_games
            mean_scores.append(mean_score)
            # plot(scores, mean_scores)


if __name__ == '__main__':
    train()