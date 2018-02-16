#include <cstdint>
#include <iostream>

int count_leading_ones(uint16_t i)
{
    int count = 0;
    for (; i & 1 && count < 16; i >>= 1)
    {
        ++count;
    }
    return count;
}

int count_leading_zeros(uint16_t i)
{
    int count = 0;
    for (; 0 == (i & 1) && count < 16; i >>= 1)
    {
        ++count;
    }
    return count;
}

int count_trailing_ones(uint16_t i)
{
    int count = 0;
    for (; i & 0x8000 && count < 16; i <<= 1)
    {
        ++count;
    }
    return count;
}

int count_trailing_zeros(uint16_t i)
{
    int count = 0;
    for (; 0 == (i & 0x8000) && count < 16; i <<= 1)
    {
        ++count;
    }
    return count;
}

int main() {
    std::cout << count_leading_ones(0x00FF) << std::endl; // Should be 5
    std::cout << count_leading_zeros(0xFF00) << std::endl; // Should be 4
    std::cout << count_trailing_ones(0xFF00) << std::endl; // Should be 3
    std::cout << count_trailing_zeros(0x00FF) << std::endl; // Should be 2

    return 0;
}

