// reader.cpp
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <thread>
struct DataShared
{
    pthread_mutex_t mutex;
    int shared_int = -1;
};
int main() {
    const char* shm_name = "/my_shared_memory";
    const size_t SIZE = sizeof(DataShared);

    // Mở shared memory đã được tạo từ writer
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Ánh xạ vùng nhớ
    void* ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Đọc giá trị
    DataShared* data = (DataShared*)ptr;

    while (true) {
        pthread_mutex_lock(&data->mutex);
        std::cout << "Reader: value = " << data->shared_int << std::endl;
        data->shared_int--;
        pthread_mutex_unlock(&data->mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Unmap và đóng
    munmap(ptr, SIZE);
    close(shm_fd);

    // Xoá shared memory sau khi xong
    shm_unlink(shm_name);
    return 0;
}
