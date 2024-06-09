// Process2.cpp
#include "MessageCommunicator.hpp"
#include <iostream>

void onMessageReceived(const std::string& message) {
    std::cout << "Process 2 received: " << message << std::endl;
}

int main() {
    try {
        MessageCommunicator communicator(L"Process2Class", onMessageReceived);
        //communicator.sendMessage(L"Process1Class", "Hello from Process 2!");
        communicator.runMessageLoop();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}