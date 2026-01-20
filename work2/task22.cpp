#include <iostream>
#include <thread>
#include <string>

int main(){
    std::string byref = "Hello";
    std::thread t(
        // lambda function
        [&](std::string str)->void{
            std::cout << byref << ", " << str << "!" << std::endl;
        },
        // arguments
        "lambda"
    );
    t.join();
}

/*
g++ main.cpp -std=c++17 -o main && ./main
*/

