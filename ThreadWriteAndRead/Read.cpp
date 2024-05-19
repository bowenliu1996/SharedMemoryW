// reader.cpp
#include <iostream>
#include <thread>
#include "sharedMemoryW.h"

// 线程函数，用于接收消息
void receiveMessages(SharedMemory& sharedMemory) {
    while (true) {
        sharedMemory.wait();  // 等待数据写入
        std::wstring data = sharedMemory.read();  // 读取数据
        std::wcout << L"Received data: " << data << std::endl;
        // 重置事件
        sharedMemory.signal();
    }
}

int main() {
    try {
        SharedMemory sharedMemory(L"Global\\{D998E69F-E3D4-4B52-9DBA-752AAB9D50F5}", 1024);

        // 打开现有的共享内存
        if (!sharedMemory.open()) {
            std::wcerr << L"Failed to open shared memory." << std::endl;
            return 1;
        }

        // 创建接收消息的线程
        std::thread receiverThread(receiveMessages, std::ref(sharedMemory));

        // 主线程等待接收线程完成（在这个例子中，主线程会一直运行）
        receiverThread.join();

        sharedMemory.close();
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
