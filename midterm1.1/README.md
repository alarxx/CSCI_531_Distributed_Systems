# Midterm 1

## Task 1
Threads from a vector of jthreads print messages “Hello from thread, i, thread id”,
where i is a loop index in range [0, number of cores) and thread id is a unique identifier of each thread.
The message and the index are passed as arguments to a separate function on a thread call.
Wrap statements in the separate function into a try catch block.
Refer to chapter 2, but use jthread instead of thread and compile using c++20.
Try to add the declared thread with the arguments into the threads vector using push_back and emplace_back functions.
What is the difference ?

## Task 2
Implement the listing 3.2 Accidentally passing out a reference to protected data without any additional threads.
The task is to access variable values in some_class from the main function.

## Task 3
There are 100 files with different file names in a directory.
The application launches N threads to rename the files from 1 to 100. N equals the number of cores.
For example, some of the file names were: city.png, classmates.jpg, park.jpg etc.
In the end their names should become 1.png, 2.jpg, 3.jpg etc.
Declare a shared variable to keep track of the renamed files to avoid the race conditions.
Use mutex, shared_mutex, locks, unique_locks while renaming the files and updating the shared variable meaningfully.

## Task 4
Implement the parallel quicksort using futures.
