/*
    SPDX-License-Identifier: MPL-2.0
    --------------------------------
    This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
    If a copy of the MPL was not distributed with this file,
    You can obtain one at https://mozilla.org/MPL/2.0/.

    Provided “as is”, without warranty of any kind.

    Copyright © 2026 Alar Akilbekov. All rights reserved.
 */
#include <stack>
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>

template <class T>
class ThreadsafeStack {
private:
    std::mutex _m;
    std::stack<T> _stk;
public:
    void push(T v) {
        std::lock_guard<std::mutex> lk(_m);
        _stk.push(std::move(v));
    }

    size_t size() const {
        std::lock_guard<std::mutex> lk(_m);
        return _stk.size();
    }

    friend void deadlock_swap(ThreadsafeStack& a, ThreadsafeStack& b) {
        if (&a == &b) return;

        std::unique_lock<std::mutex> lk1(a._m);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::unique_lock<std::mutex> lk2(b._m);

        std::swap(a._stk, b._stk);
    }

    friend void good_swap(ThreadsafeStack& a, ThreadsafeStack& b) {
        if (&a == &b) return;
        std::scoped_lock guard(a._m, b._m); // lock order
        std::swap(a._stk, b._stk);
    }
};

int main() {
    ThreadsafeStack<int> s1;
    ThreadsafeStack<int> s2;

    s1.push(1); s1.push(2); s1.push(3);
    s2.push(10); s2.push(20); s2.push(30);

    std::thread t1([&]()->void{
        std::cout << "t1: swap(s1,s2)\n";
        deadlock_swap(s1, s2);
        // good_swap(s1, s2);
        std::cout << "t1 done\n";
    });
    std::thread t2([&]()->void{
        std::cout << "t2: swap(s2,s1)\n";
        deadlock_swap(s2, s1);
        // good_swap(s2, s1);
        std::cout << "t2 done\n";
    });

    t1.join();
    t2.join();

    std::cout << "completed" << std::endl;
}


