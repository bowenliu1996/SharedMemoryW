#include <windows.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>

class SharedMemory {
public:
    SharedMemory(const std::wstring& name, size_t size);
    ~SharedMemory();

    bool create();
    bool open();
    void write(const std::wstring& data);
    std::wstring read();
    void close();
    void signal();  // ����֪ͨ��һ������������д��
    void wait();    // ���ڵȴ���һ������д������

private:
    std::wstring name_;
    size_t size_;
    HANDLE hFile_;
    HANDLE hMapFile_;
    LPVOID lpBase_;
    HANDLE hEvent_;

    void cleanup();
    void checkAndThrow(bool condition, const std::wstring& errorMessage);
};

