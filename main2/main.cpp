// reader.cpp
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <thread>
#include <opencv2/opencv.hpp>
#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080
#define MAX_CHANNELS 3
#define MAX_IMAGE_SIZE (MAX_WIDTH * MAX_HEIGHT * MAX_CHANNELS)
typedef unsigned char uchar;
typedef struct {
    pthread_mutex_t mutex;
    int rows;
    int cols;
    int type;
    size_t size;
    uchar image[MAX_IMAGE_SIZE];
}SharedImage;
int main() {
    const char* shm_name = "/my_shared_memory";
    const size_t SIZE = sizeof(SharedImage);

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
    SharedImage* data = (SharedImage*)ptr;

    while (true) {
        pthread_mutex_lock(&data->mutex);
        if (data->rows > 0 && data->cols > 0 && data->size > 0) {
            // Tạo cv::Mat trỏ trực tiếp vào shared memory
            cv::Mat img(data->rows, data->cols, data->type, data->image);

            cv::imshow("Shared Memory Viewer", img);
        }
        std::cout << "Reader: value = " << sizeof(data->image) << std::endl;
        pthread_mutex_unlock(&data->mutex);
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (cv::waitKey(1) == 27) break; // ESC để thoát
    }

    // Unmap và đóng
    munmap(ptr, SIZE);
    close(shm_fd);

    // Xoá shared memory sau khi xong
    shm_unlink(shm_name);
    cv::destroyAllWindows();
    return 0;
}
