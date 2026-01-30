/*
    SPDX-License-Identifier: MPL-2.0
    --------------------------------
    This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
    If a copy of the MPL was not distributed with this file,
    You can obtain one at https://mozilla.org/MPL/2.0/.

    Provided “as is”, without warranty of any kind.

    Copyright © 2026 Alar Akilbekov. All rights reserved.
*/

#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>

namespace fs = std::filesystem;


// class State {};
// std::atomic<int> id(0);
std::mutex id_mtx;
int id = 0;

// std::shared_mutex renamed_mtx;
std::shared_timed_mutex renamed_mtx;
std::vector<bool> renamed;


void writer(
    const std::vector<fs::path> & files,
    const fs::path & dir
){
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // int _id = next_file.fetch_add(1);
        int _id;
        {
            std::unique_lock lk(id_mtx);
            _id = id++;
        }
        if (_id >= files.size()) return;


        fs::path new_name = dir / (std::to_string(_id + 1) + files[_id].extension().string());

        // fs::rename moves file
        fs::copy_file(files[_id], new_name, fs::copy_options::overwrite_existing);


        // {
        //     std::unique_lock shlk(renamed_mtx);
        //     renamed[_id] = true;
        // }
        while(true){
            std::unique_lock shlk(renamed_mtx, std::defer_lock);

            if (shlk.try_lock_for(std::chrono::nanoseconds(1))) {
                renamed[_id] = true;
                std::cout << "writer(" << _id << files[_id].extension().string() << "):wrote\n";
                shlk.unlock();
                break;
            } else {
                std::cout << "writer(" << _id << files[_id].extension().string() << "):timeout, will retry\n";
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            }
        }
    }
}


std::atomic<int> monitor_i(0);

void monitor(const std::vector<fs::path> & files, int tid) {
    for(unsigned long i = 0; true; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::shared_lock shlk(renamed_mtx);
        std::cout << "monitor(" << tid << "):locked\n";
        int nrenamed = 0;
        for(bool v : renamed){ // renamed.size()
            if (v){
                ++nrenamed;
            }
        }
        std::cout << "monitor(" << tid << "|" << monitor_i.fetch_add(1) << "):" << nrenamed << "\n";
        if(nrenamed == files.size() && i > 5000){
            break;
        }
    }
}


int main() {
    // fs::path srcdir = "./CMNIST";
    fs::path srcdir = "./KOHTD";
    fs::path dstdir = "./renamed";
    fs::create_directories(dstdir); // mkdir -p <dir>

    std::vector<fs::path> files;
    for(auto& entry : fs::directory_iterator(srcdir)){ // std::filesystem::directory_entry
        if (entry.is_regular_file()){
            // entry.path().extension().string()
            files.push_back(entry.path());
        }
    }

    renamed.resize(files.size(), false);

    int ncores = std::thread::hardware_concurrency();
    // ncores can be less of greater than the files.size()

    std::vector<std::jthread> threads;

    threads.emplace_back(monitor, std::ref(files), 0);
    threads.emplace_back(monitor, std::ref(files), 1);

    for (int i = 0; i < ncores; ++i){
        threads.emplace_back(writer, std::ref(files), dstdir);
    }

    // No join
}

/*
g++ main.cpp -o main.out -std=c++20 && ./main.out
*/
