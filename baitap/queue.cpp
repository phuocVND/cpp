#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

template <typename T>

class Queue{

private:
    T *head;
    T *rear;
    int capacity;
public:
    Queue();
    ~Queue();

    void pop();
    void push(T value);
    T front();
    T back();
    bool empty();
};

template <typename T>
Queue<T>::Queue(){
    this->capacity = 0;
}

template <typename T>
Queue<T>::~Queue(){
    delete[] head;
}

template <typename T>
bool Queue<T>::empty(){
    if(this->capacity > 0){
        return false;
    }
    return true;
}

template <typename T>
void Queue<T>::pop() {
    if(!empty()){
        capacity--;
        cout << "couterrrrr: " << capacity << endl;
        this->head--;
        // if(capacity > 0){

        // }
        // else{
        //     cout << "couterrrrr: " << capacity << endl;
        // }
    }
    else{
        this->rear = 0;
        this->head = this->rear;
        cout << "Rỗng: " << capacity << endl;
    }
}
template <typename T>
void Queue<T>::push(T value) {
    // cout << "check empty: "<< capacity << std::endl;
    if(empty()){
        capacity = 1;
        this->rear = new T[capacity];
        this->head = this->rear;
        *this->head = value;
        return;
    }
    else{
        capacity++;
        this->rear = ([this, value]()-> T*{
            int i = 0;
            T *arr = new T[capacity];

            while(i < capacity - 1){
                *(arr + i) = *(rear + i);
                i++;
            }

            delete[] this->rear;
            this->rear = nullptr;
            return arr;
        })();
        this->head = this->rear + capacity - 1;
        *this->head = value;
    }
}

template <typename T>
T Queue<T>::back() {
    if(empty()){
        cout << "Rỗng: " << capacity << endl;
    }
    return *head;
}

template <typename T>
T Queue<T>::front() {
    if(empty()){
        cout << "Rỗng: " << capacity << endl;
        return;
    }
    return *rear;
}

int main() {

    Queue<int> q;

    q.push(100);
    q.push(200);
    q.push(300);

    cout << "Front: " << q.back() << endl; // 300 (đúng)
    q.pop(); // head-- (sai logic)

    cout << "Front sau pop: " << q.back() << endl; // ❌ Không còn đảm bảo đúng
    q.pop();
    cout << "Front cuối cùng: " << q.back() << endl; // ⚠️ GÂY LỖI hoặc IN RÁC
    q.pop();

    // ❗❗ Bây giờ head đã bị head-- nhiều lần
    // → trỏ ra vùng nhớ trước mảng
    cout << "Front cuối cùng: " << q.back() << endl; // ⚠️ GÂY LỖI hoặc IN RÁC

    return 0;
}

