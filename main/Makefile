# Trình biên dịch và cờ
CC = g++
CFLAGS = -Wall -g -std=c++11 `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

# Tên chương trình
TARGET = main

# File nguồn và đối tượng
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

# Mặc định: build và chạy
all: clean $(TARGET) run

# Liên kết file object thành chương trình
$(TARGET): $(OBJ)
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Biên dịch từng file .cpp thành .o
%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

# Chạy chương trình
run: $(TARGET)
	@echo "--------------------------------------------------"
	@./$(TARGET)

# Dọn dẹp
clean:
	@rm -f $(TARGET) $(OBJ)
