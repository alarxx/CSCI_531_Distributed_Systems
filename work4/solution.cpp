/*
    SPDX-License-Identifier: MPL-2.0
    --------------------------------
    This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
    If a copy of the MPL was not distributed with this file,
    You can obtain one at https://mozilla.org/MPL/2.0/.

    Provided “as is”, without warranty of any kind.

    Copyright © 2025-2026 Alar Akilbekov. All rights reserved.
 */

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <vector>
#include <queue>


class Writer {
/*
    Message Queue
*/
private:
    std::queue<std::string> que;
    std::mutex m;
    std::condition_variable cv;
    std::ofstream outputFile;
    std::thread daemon;
    bool isRunning = true;
public:
    Writer(std::string filePath){
        outputFile.open(filePath);
        daemon = std::thread([this]()->void{
            listener();
        });
    }
    ~Writer(){
        isRunning = false;
        cv.notify_all();
        if (daemon.joinable()) {
            daemon.join();
        }
        outputFile.close();
    }

    void write(std::string line) {
        { // RAII
            std::lock_guard<std::mutex> lk(m);
            que.push(line);
        }
        cv.notify_one();
    }

    void listener(){
        while(isRunning){
            std::unique_lock<std::mutex> lk(m);
            // while(!ready){} // instead of this
            // sleeps if empty, locks otherwise and processes queue
            cv.wait(lk, [&]()->bool{ return !que.empty() || !isRunning; });

            if (!isRunning && que.empty()) {
                break; // clean shutdown
            }

            std::string line = que.front();
            que.pop();
            // write
            outputFile << line << "\n";
            lk.unlock();
        }
    }
};


void thread_function(int thread_i, Writer& writer, std::string word, std::string filePath){
    std::cout << "thread(" << thread_i << "): " << filePath << std::endl;

    std::ifstream file(filePath);
    std::string fullLine;

    int line_i = 0;
    while (std::getline(file, fullLine)) {
        line_i++;
        if (fullLine.find(word) != std::string::npos) {
            std::string line = "Found on thread(" + std::to_string(thread_i) + "): " + std::to_string(line_i);
            writer.write(line);
        }
    }
}

int main(){
    unsigned int M = std::thread::hardware_concurrency();
    std::cout << "Number of hardware threads available: " << M << std::endl;

    std::string word;
    std::cout << "Enter the word: "; // Jack
    std::cin >> word;
    std::cout << std::endl;

    std::vector<std::string> txtFiles;
    std::string rootDir = "./";
    for (const auto & entry : std::filesystem::directory_iterator(rootDir)){
        if (entry.is_regular_file()) {
            if (entry.path().extension().string() == ".txt"
                                    && entry.path() != "./result.txt") {
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

