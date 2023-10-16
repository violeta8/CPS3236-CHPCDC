#include <iostream>
#include <thread>

void task1(std::string msg)
{
    std::cout << "task1 says: " << msg;
}

int main()
{
    std::thread t1(task1, "Hello");
    t1.join();

    return 0;
}


