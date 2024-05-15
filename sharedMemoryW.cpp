
#include "sharedMemoryW.h"
// ���캯��
SharedMemory::SharedMemory(const std::wstring& name, size_t size)
    : name_(name), size_(size), hFile_(NULL), hMapFile_(NULL), lpBase_(NULL), hEvent_(NULL) {}

// ��������
SharedMemory::~SharedMemory() {
    close();
}

// �����ļ�ӳ�������¼�����
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

// �����е��ļ�ӳ�������¼�����
bool SharedMemory::open() {
    hMapFile_ = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name_.c_str());
    checkAndThrow(hMapFile_ != NULL, L"Unable to open file mapping object.");

    lpBase_ = MapViewOfFile(hMapFile_, FILE_MAP_ALL_ACCESS, 0, 0, size_);
    checkAndThrow(lpBase_ != NULL, L"Unable to map view of file.");

    hEvent_ = OpenEventW(EVENT_ALL_ACCESS, FALSE, (name_ + L"_Event").c_str());
    checkAndThrow(hEvent_ != NULL, L"Unable to open event object.");

    return true;
}

// д�����ݵ�ӳ���ڴ�
void SharedMemory::write(const std::wstring& data) {
    if (lpBase_ != NULL) {
        memcpy(lpBase_, data.c_str(), (data.size() + 1) * sizeof(wchar_t));  // ������ֹ��
        signal(); // ֪ͨ��һ������������д��
    }
}

// ��ȡ���ݴ�ӳ���ڴ�
std::wstring SharedMemory::read() {
    if (lpBase_ != NULL) {
        return std::wstring(static_cast<wchar_t*>(lpBase_));
    }
    return L"";
}

// �ر��ļ�ӳ����¼�����
void SharedMemory::close() {
    cleanup();
}

// ����֪ͨ��һ������������д��
void SharedMemory::signal() {
    if (hEvent_ != NULL) {
        SetEvent(hEvent_);
    }
}

// ���ڵȴ���һ������д������
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