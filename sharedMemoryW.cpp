
#include "sharedMemoryW.h"
// 构造函数
SharedMemory::SharedMemory(const std::wstring& name, size_t size)
    : name_(name), size_(size), hFile_(NULL), hMapFile_(NULL), lpBase_(NULL), hEvent_(NULL) {}

// 析构函数
SharedMemory::~SharedMemory() {
    close();
}

// 创建文件映射对象和事件对象
bool SharedMemory::create() {
    hFile_ = CreateFileW(name_.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    checkAndThrow(hFile_ != INVALID_HANDLE_VALUE, L"Unable to create file.");

    hMapFile_ = CreateFileMappingW(hFile_, NULL, PAGE_READWRITE, 0, static_cast<DWORD>(size_), name_.c_str());
    checkAndThrow(hMapFile_ != NULL, L"Unable to create file mapping object.");

    lpBase_ = MapViewOfFile(hMapFile_, FILE_MAP_ALL_ACCESS, 0, 0, size_);
    checkAndThrow(lpBase_ != NULL, L"Unable to map view of file.");

    hEvent_ = CreateEventW(NULL, TRUE, FALSE, (name_ + L"_Event").c_str());
    checkAndThrow(hEvent_ != NULL, L"Unable to create event object.");

    return true;
}

// 打开现有的文件映射对象和事件对象
bool SharedMemory::open() {
    hMapFile_ = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name_.c_str());
    checkAndThrow(hMapFile_ != NULL, L"Unable to open file mapping object.");

    lpBase_ = MapViewOfFile(hMapFile_, FILE_MAP_ALL_ACCESS, 0, 0, size_);
    checkAndThrow(lpBase_ != NULL, L"Unable to map view of file.");

    hEvent_ = OpenEventW(EVENT_ALL_ACCESS, FALSE, (name_ + L"_Event").c_str());
    checkAndThrow(hEvent_ != NULL, L"Unable to open event object.");

    return true;
}

// 写入数据到映射内存
void SharedMemory::write(const std::wstring& data) {
    if (lpBase_ != NULL) {
        memcpy(lpBase_, data.c_str(), (data.size() + 1) * sizeof(wchar_t));  // 包括终止符
        signal(); // 通知另一个进程数据已写入
    }
}

// 读取数据从映射内存
std::wstring SharedMemory::read() {
    if (lpBase_ != NULL) {
        return std::wstring(static_cast<wchar_t*>(lpBase_));
    }
    return L"";
}

// 关闭文件映射和事件对象
void SharedMemory::close() {
    cleanup();
}

// 用于通知另一个进程数据已写入
void SharedMemory::signal() {
    if (hEvent_ != NULL) {
        SetEvent(hEvent_);
    }
}

// 用于等待另一个进程写入数据
void SharedMemory::wait() {
    if (hEvent_ != NULL) {
        WaitForSingleObject(hEvent_, INFINITE);
    }
}

void SharedMemory::cleanup() {
    if (lpBase_ != NULL) {
        UnmapViewOfFile(lpBase_);
        lpBase_ = NULL;
    }
    if (hMapFile_ != NULL) {
        CloseHandle(hMapFile_);
        hMapFile_ = NULL;
    }
    if (hFile_ != NULL) {
        CloseHandle(hFile_);
        hFile_ = NULL;
    }
    if (hEvent_ != NULL) {
        CloseHandle(hEvent_);
        hEvent_ = NULL;
    }
}

void SharedMemory::checkAndThrow(bool condition, const std::wstring& errorMessage) {
    if (!condition) {
        DWORD errorCode = GetLastError();
        throw std::runtime_error(std::string(errorMessage.begin(), errorMessage.end()) + " Error code: " + std::to_string(errorCode));
    }
}