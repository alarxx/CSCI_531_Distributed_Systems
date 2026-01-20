#include <iostream>
#include <thread>
#include <string>


class background_task {
public:
    void operator()(std::string str) const;
};

void background_task::operator()(std::string str) const {
    std::cout << "background_task: " << str << std::endl;
}


int main(){
    {
        background_task f;
        std::thread t(f, "first");
        t.join();
    }

    {
        std::thread t((background_task()), "second");
        t.join();
    }

    {
        std::thread t{background_task(), "third"};
        t.join();
    }
}

/*
g++ main.cpp -std=c++17 -o main && ./main
*/

