#include <windows.h>
#include <iostream>
#include <string>
#include "sharedMemoryW.h" 

void CreateChildProcess(const std::wstring& sharedMemoryName, size_t sharedMemorySize) {
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstring commandLine = L"ChildProcess.exe " + sharedMemoryName + L" " + std::to_wstring(sharedMemorySize);

    if (!CreateProcessW(NULL, &commandLine[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::wcerr << L"CreateProcess failed (" << GetLastError() << L")." << std::endl;
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() {
    const std::wstring sharedMemoryName = L"SharedMemoryExample";
    const size_t sharedMemorySize = 1024;

    SharedMemory sharedMemory(sharedMemoryName, sharedMemorySize);

    try {
        sharedMemory.create();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    CreateChildProcess(sharedMemoryName, sharedMemorySize);

    std::wstring message = L"Hello from Parent Process";
    sharedMemory.write(message);

    std::wcout << L"Parent process waiting for child process to read the message..." << std::endl;
    sharedMemory.wait();  // 等待子进程读取数据

    std::wcout << L"Parent process read: " << sharedMemory.read() << std::endl;

    sharedMemory.close();
    system("pause");
    return 0;
}
