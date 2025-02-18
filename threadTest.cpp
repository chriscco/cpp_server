#include "inc/threadpool.h"
#include <string>
#include <iostream>

void print(int i, double j, const char* str1, std::string str2) {
    printf("i: %d, j: %.2f, str1: %s, str2: %s\n", i, j, str1, str2.c_str());
}

void test() {
    printf("helllllllo\n");
}

int main() {
    ThreadPool* pool = new ThreadPool(3);
    std::function<void()> func = std::bind(print, 1, 3.14, "hello", std::string("world"));
    pool->add(func);
    func = test;
    pool->add(func);
    delete pool;
    return 0;
}