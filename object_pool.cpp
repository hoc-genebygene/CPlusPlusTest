#include <iostream>
#include <memory>
#include <vector>

#include "boost/pool/pool.hpp"
#include "boost/pool/singleton_pool.hpp"

size_t A_BLOCK_SIZE = 5;

class A {
public:
    void print() {
        std::cout << "Hello World!" << std::endl;
    }

    static void * operator new(std::size_t sz);
    static void * operator new[](std::size_t sz);
    static void operator delete(void * ptr, std::size_t sz) noexcept;
    static void operator delete[](void * ptr, std::size_t sz) noexcept;

    static A** unused_a_block;

public:
    A() { std::cout << "A constructor" << std::endl; }
    ~A() { std::cout << "A destructor" << std::endl; }
};

A** A::unused_a_block = nullptr;

using APool = boost::singleton_pool<A, sizeof(A)>;

void * A::operator new(std::size_t sz) {
    std::cout << "A custom new" << std::endl;
    static size_t current_unused_index = A_BLOCK_SIZE;

    if (current_unused_index != A_BLOCK_SIZE) {
        static A * read_ret = nullptr;
        read_ret = &(*unused_a_block)[current_unused_index];
        ++current_unused_index;
        return read_ret;
    }

    static A * new_block = nullptr;
    new_block = static_cast<A*>(APool::ordered_malloc(A_BLOCK_SIZE));
    unused_a_block = &new_block;

    current_unused_index = 0;

    static A * read_ret = nullptr;
    read_ret = &(*unused_a_block)[current_unused_index];
    ++current_unused_index;
    return read_ret;
}

void * A::operator new[](std::size_t sz) {
    std::cout << "A custom array new" << std::endl;
    return APool::ordered_malloc(sz);
}

void A::operator delete(void * ptr, std::size_t sz) noexcept {
    std::cout << "A custom delete" << std::endl;
    APool::free(ptr);
}

void A::operator delete[](void * ptr, std::size_t sz) noexcept {
    std::cout << "A custom array delete" << std::endl;
    APool::ordered_free(ptr, sz);
}

struct ADeleter {
    void operator()(A * a) {
        APool::free(a);
    }
};

using AUniquePtr = std::unique_ptr<A, ADeleter>;

int main() {
    using AUniquePtr = std::unique_ptr<A>;

    std::vector<AUniquePtr> vec;
    vec.resize(1024);

    for (auto it = vec.begin(); it != vec.end(); ++it) {
        *it = std::make_unique<A>();
    }

    return 0;
}

