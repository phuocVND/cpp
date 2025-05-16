#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
template <typename T>
class Stack{
private:
    T *ptr;
    int capacity;
public:
    Stack();
    ~Stack();
    T top();
    void push(T value);
    void pop();
    bool empty();

};

template <typename T>
Stack<T>::Stack(){
    this->capacity = 0;
    this->ptr = nullptr;
}
template <typename T>
Stack<T>::~Stack(){
    delete[] ptr;
}

template <typename T>
bool Stack<T>::empty(){
    if(this->capacity <= 0){
        return true;
    }
    return false;
}

template <typename T>
void Stack<T>::push(T value){

    if(empty()){
        this->capacity = 1;
        this->ptr = new T[this->capacity];
        *this->ptr = value;
    }
    else{
        this->capacity++;
        this->ptr = ([this, value](){
            T *arr = new T[this->capacity];
            int i=0;
            while(i < capacity - 1){
                *(arr + i) = *(ptr + i);
                i++;
            }
            *(arr + capacity - 1) = value;
            delete[] ptr;
            return arr;
        })();
    }
}

template <typename T>
T Stack<T>::top(){
    cout << "check: "<<capacity << endl;
    if(empty()){
        cout << "Rỗng" << endl;
    }
    return *(ptr + capacity - 1);
}

template <typename T>
void Stack<T>::pop(){

    if(!empty()){
        capacity--;
    }
}
int main() {

    Stack<int> s;

    s.push(100);
    s.push(200);
    s.push(300);

    cout << "Front: " << s.top() << endl; // 300 (đúng)
    s.pop(); // head-- (sai logic)

    cout << "Front sau pop: " << s.top() << endl; // ❌ Không còn đảm bảo đúng
    s.pop();
    cout << "Front cuối cùng: " << s.top() << endl; // ⚠️ GÂY LỖI hoặc IN RÁC
    s.pop();

    cout << "Front check: " << s.top() << endl; // ⚠️ GÂY LỖI hoặc IN RÁC

    return 0;
}

