// Process1.cpp
#include "MessageCommunicator.hpp"
#include <iostream>

void onMessageReceived(const std::string& message) {
    std::cout << "Process 1 received: " << message << std::endl;
}

int main() 
{
    try {
        MessageCommunicator communicator(L"Process1Class", onMessageReceived);
        communicator.sendMessage(L"Process2Class", "Hello from Process 1!");
        communicator.runMessageLoop();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}