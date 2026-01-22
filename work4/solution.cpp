#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

struct Writer {
    std::mutex m;
    std::condition_variable cv;
    std::ofstream outputFile;

    Writer(std::string filePath){
        outputFile.open(filePath);
    }
    ~Writer(){
        outputFile.close();
    }

    void write(std::string line) {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return true; }); // locks
        // write
        outputFile << line << "\n";
        lk.unlock();
        cv.notify_one();
    }
};


void thread_function(int thread_i, Writer& writer, std::string word, std::string filePath){
    std::cout << filePath << std::endl;

    std::ifstream file(filePath);
    std::string fullLine;

    int line_i = 0;
    while (std::getline(file, fullLine)) {
        line_i++;
        if (fullLine.find(word) != std::string::npos) {
            std::string line = "Found " + std::to_string(thread_i) + " " + std::to_string(line_i);
            writer.write(line);
        }
    }
}

int main(){
    unsigned int M = std::thread::hardware_concurrency();
    std::cout << "Number of hardware threads available: " << M << std::endl;

    std::string word;
    std::cout << "Enter the word: ";
    std::cin >> word;
    std::cout << std::endl;

    std::vector<std::string> txtFiles;
    std::string rootDir = "./";
    for (const auto & entry : fs::directory_iterator(rootDir)){
        if (entry.is_regular_file()) {
            if (entry.path().extension().string() == ".txt") {
                txtFiles.push_back(entry.path());
            }
        }
    }

    if(M < txtFiles.size()){
        std::cout << "More files than expected" << std::endl;
        return 1;
    }

    Writer writer("./result.txt");

    std::vector<std::thread> threads;
    // threads.reserve(txtFiles.size());
    for(int i = 0; i < txtFiles.size(); i++){
        threads.emplace_back(thread_function, i, std::ref(writer), word, txtFiles[i]);
    }

    for (auto& t : threads) t.join();


}
