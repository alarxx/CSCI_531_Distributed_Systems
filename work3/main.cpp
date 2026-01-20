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

#include "LinkedList.tpp"

int main(){
    std::cout << "start ------------" << std::endl;

    LinkedList<std::string> ll;

    ll.addLast("A");
    ll.addLast("B");
    ll.addLast("C");

    // ll.remove(String("A"));
    std::cout << "removed: " << ll.removeFirst() << std::endl;
    std::cout << "removed: " << ll.removeFirst() << std::endl;
    std::cout << "removed: " << ll.removeFirst() << std::endl;

    for(int i = 0; i < ll.size(); i++){
        std::cout << ll.get(i) << std::endl;
    }

    std::cout << "end ------------" << std::endl;
}
