CC = g++
CFLAGS = -Wall -g
TARGET = main

SRC = main.cpp tcpServer.cpp
OBJ = $(SRC:.cpp=.o)

# Target 'all' sẽ phụ thuộc vào các target khác
all:  clean run $(TARGET)

# Biên dịch và liên kết chương trình
$(TARGET): $(OBJ)
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Biên dịch các file .cpp thành file .o
%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

# Chạy chương trình
run: $(TARGET)
	@echo "--------------------------------------------------"
	@./$(TARGET)

# Dọn dẹp
clean:
	@rm -f $(TARGET) $(OBJ)
