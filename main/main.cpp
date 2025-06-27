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

    SharedImage* data = (SharedImage*)ptr;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Không mở được camera!\n";
        return 1;
    }
    cv::Mat frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        // Ghi vào shared memory có mutex bảo vệ
        pthread_mutex_lock(&data->mutex);

        data->rows = frame.rows;
        data->cols = frame.cols;
        data->type = frame.type();
        data->size = frame.total() * frame.elemSize();

        if (data->size <= MAX_IMAGE_SIZE) {
            std::memcpy(data->image, frame.data, data->size);
            std::cout << "Writer: wrote image (" << data->cols << "x" << data->rows << "), size = " << data->size << " bytes\n";
        } else {
            std::cerr << "Frame quá lớn!\n";
        }

        pthread_mutex_unlock(&data->mutex);

        // std::this_thread::sleep_for(std::chrono::milliseconds(30)); // giảm CPU
        if (cv::waitKey(1) == 27) break; // ESC để thoát
    }

    std::cout << "Writer: update shared memory.\n";

    // Không xóa shm ở đây!
    munmap(ptr, SIZE);
    close(shm_fd);
    cap.release(); // Giải phóng camera
    cv::destroyAllWindows();
    return 0;
}
