#include <windows.h>
#include <iostream>
#include <string>
#include "sharedMemoryW.h" // 假设SharedMemory类的代码放在这个头文件中

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 3) {
        std::wcerr << L"Usage: ChildProcess <SharedMemoryName> <SharedMemorySize>" << std::endl;
        return 1;
    }

    std::wstring sharedMemoryName = argv[1];
    size_t sharedMemorySize = std::stoull(argv[2]);

    SharedMemory sharedMemory(sharedMemoryName, sharedMemorySize);

    try {
        sharedMemory.open();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::wcout << L"Child process read: " << sharedMemory.read() << std::endl;

    std::wstring response = L"Hello from Child Process";
    sharedMemory.write(response);

    std::wcout << L"Child process waiting for parent process to read the message..." << std::endl;
    sharedMemory.signal();  // 通知父进程数据已写入

    sharedMemory.close();

    system("pause");
    return 0;
}