/*
    SPDX-License-Identifier: MPL-2.0
    --------------------------------
    This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
    If a copy of the MPL was not distributed with this file,
    You can obtain one at https://mozilla.org/MPL/2.0/.

    Provided “as is”, without warranty of any kind.

    Copyright © 2026 Alar Akilbekov. All rights reserved.
 */
#include <string>
#include <thread>

#include "LinkedList.tpp"

int main(){
    for (int epoch = 1; epoch <= 2000000; ++epoch) {
        std::cout << "start ------------" << std::endl;

        LinkedList<std::string> ll;

        ll.addLast("A");
        ll.addLast("B");
        ll.addLast("C");

        // ll.remove(String("B"));

        std::thread t1([&]()->void{
            bool ok = ll.remove("B");
            std::cout << (std::string("[t1] remove(B) -> ") + (ok ? "true" : "false")) << "\n";
        });

        std::thread t2([&]()->void{
            bool ok = ll.remove("C");
            std::cout << (std::string("[t2] remove(C) -> ") + (ok ? "true" : "false")) << "\n";
        });

        t1.join();
        t2.join();

        // std::cout << ll.getFirst() << std::endl;
        // std::cout << ll.getLast() << std::endl;

        for(int i = 0; i < ll.size(); i++){
            std::cout << ll.get(i) << std::endl;
        }
        std::cout << "end ------------" << std::endl;
    }
/*
start ------------
Found node: B(0x564f64852340), next: 0x564f64852300, prev: 0x564f648522c0
deleted 0x564f64852340
[t1] remove(B) -> true
Found node: C(0x564f64852300), next: 0, prev: 0x564f648522c0
deleted 0x564f64852300
[t2] remove(C) -> true
A
end ------------
deallocate Node(A){NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE}

start ------------
Found node: B(0x564f64852300), next: 0x564f64852340, prev: 0x564f648522c0
deleted 0x564f64852300
Found node: C(0x564f64852340), next: 0, prev: 0x564f648522c0
deleted 0x564f64852340
[t2] remove(C) -> true
[t1] remove(B) -> true
A
Segmentation fault (core dumped)
*/
}
