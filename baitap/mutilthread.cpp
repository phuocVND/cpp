#include <iostream>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <chrono>
#include <vector>
struct DataThread
{
    std::condition_variable cv;
    std::mutex mtx;
    uint32_t balance = 0;
    int turn = -1;
    std::vector<pthread_t> listPthread;
    int numberThread = 0;
    bool stop = false;
};

void* helloThread(void *arg){
    DataThread* data = (DataThread*) arg;
    while(!data->stop){
        std::unique_lock<std::mutex> lock(data->mtx);
        data->cv.wait(lock, [&data] ()
            {
                return data->listPthread[data->turn] == pthread_self();
            }
        );
        if (!data->stop) {
            data->balance++;
            std::cout << "Im thread " << data->turn+1 << ": " << data->balance << std::endl;
        }
        data->turn = (data->turn + 1) % (data->numberThread);
        data->stop = data->balance >= 20;
        data->cv.notify_all();
    }

    return NULL;
}

int main() {
    DataThread* data = new DataThread;
    data->numberThread = 10;
    std::cout << "main Thread" << std::endl;
    pthread_t thread_id[data->numberThread];
    for(int i=0;i < data->numberThread;i++){
        pthread_create(&thread_id[i], NULL, helloThread, data);
        data->listPthread.push_back(thread_id[i]);
    }

    data->mtx.lock();
    data->turn = 0;
    data->cv.notify_all();
    data->mtx.unlock();

    for(int i=0;i < data->numberThread; i++){
        pthread_join(thread_id[i], NULL);
    }
    std::cout << "main Thread close" << std::endl;
    delete data;
    return 0;
}
