#pragma once
#include <string>
#include <iostream>

class Buffer {
private:
    std::string _buf; 
public: 
    Buffer();
    ~Buffer();
    void append(const char* str, int size);
    ssize_t size();
    const char* c_str();
    void clear();
    void getline();
    void setBuffer(const char* buf);
};