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
    if(t1.joinable()){
        t1.join();
    }
    if(t2.joinable()){
        t2.join();
    }
}

/*
g++ main.cpp -std=c++17 -o main && ./main
*/
