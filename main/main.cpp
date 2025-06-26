// writer.cpp
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <mutex>
#include <pthread.h>
#include <chrono>
#include <thread>
struct DataShared
{
    pthread_mutex_t mutex;
    int shared_int = -1;
};


int main() {
    const char* shm_name = "/my_shared_memory";
    const size_t SIZE = sizeof(DataShared);

    // Tạo hoặc mở shared memory
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    // Cài đặt kích thước
    ftruncate(shm_fd, SIZE);
    // Ánh xạ vào bộ nhớ
    void* ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    DataShared* data = (DataShared*)ptr;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    while (true) {
        pthread_mutex_lock(&data->mutex);
        data->shared_int++;
        std::cout << "Writer: wrote " << data->shared_int << std::endl;
        pthread_mutex_unlock(&data->mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Writer: update shared memory.\n";

    // Không xóa shm ở đây!
    munmap(ptr, SIZE);
    close(shm_fd);
    return 0;
}
