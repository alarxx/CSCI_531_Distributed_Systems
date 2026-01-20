#include <iostream>
#include <thread>
#include <string>
#include <fstream>


void do_something(int i){
    std::cout << "i" << std::endl;
}

struct func{
    int i;
    func(int i_): i(i_) {}

    void operator()(){
        std::ofstream outFile("example.txt");
        if (!outFile.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
        }
        for(unsigned j=0; j<1000000; ++j){
            outFile << i << std::endl;
        }
        outFile.close();
    }

};

void oops(){
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    // my_thread.join();
    my_thread.detach();
}

int main(){

    // std::ofstream outFile("example.txt");
    // if (!outFile.is_open()) {
    //     std::cerr << "Error opening file!" << std::endl;
    //     return 1;
    // }
    // for(unsigned j=0; j<1000000; ++j){
    //     outFile << j << std::endl;
    // }
    // outFile.close();

    oops();
    std::cout << "end program" << std::endl;
}

