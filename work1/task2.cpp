#include <iostream>
#include <thread>

void hello(){
    std::cout << "Hello Concurrent World\n";
}

void goodbye(){
    std::cout << "Goodbye Concurrent World\n";
}

int main(){
    std::thread t1(hello);
    std::thread t2(goodbye);
    t1.join();
    t2.join();
}

