// writer.cpp
#include <windows.h>
#include <sddl.h>
#include <iostream>
#include <string>
#include "sharedMemoryW.h"


int main() {
    try {
        SharedMemory sharedMemory(L"Global\\{D998E69F-E3D4-4B52-9DBA-752AAB9D50F5}", 1024);

        // 创建新的共享内存


        if (!sharedMemory.create()) {
            std::wcerr << L"Failed to create shared memory." << std::endl;
            return 1;
        }


        std::wstring input;
        while (true) {
            std::wcout << L"Enter data to write (type 'exit' to quit): ";
            std::getline(std::wcin, input);
            if (input == L"exit") {
                break;
            }
            sharedMemory.write(input);  // 写入数据到共享内存
        }

        sharedMemory.close();
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
