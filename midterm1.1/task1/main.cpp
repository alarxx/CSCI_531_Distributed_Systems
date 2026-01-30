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
#include <thread>
#include <string>
#include <exception>

void thread_func(std::string msg, int i) {
    try {
        throw std::runtime_error("Custom Error from thread_func");
        std::cout << msg << ", i = " << i << ", thread id = " << std::this_thread::get_id() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in thread " << i << ": " << e.what() << '\n';
    }
}

int main() {
    int n = std::thread::hardware_concurrency();

    std::vector<std::jthread> threads;
    // threads.reserve(n);

    for (int i = 0; i < n; ++i) {
        // try {
            std::jthread t(thread_func, "Hello from thread", i);
            // threads.push_back(t);
            // threads.emplace_back(t);
            threads.push_back(std::move(t));

            // threads.emplace_back(std::move(t));

            // threads.emplace_back(thread_func, "Hello from thread", i);
        // }
        // catch (const std::exception& e) {
        //     std::cerr << "Exception in thread " << i << ": " << e.what() << '\n';
        // }
    }

    // No join

    return 0;
}

/*
g++ main.cpp -o main.out -std=c++20 && ./main.out
*/
