/**
 * # Warning: proper memory alignment needed （内存对齐）
 * 内存对齐很重要
 * 1. 什么是内存对齐？what
 * - 内存对齐是不同变量首地址对齐，在下面 float8_t 的例子中，即 float8_t 变量首地址是 32 的倍数
 *
 * 2. 为什么需要内存对齐？why
 * - 通常 SIMD 场景下需要内存对齐
 * - 如果不满足这个要求，并且正好数据块长度超出寄存器长度，对于cpu来说，需要“分页”读取这个数据块，有可能导致性能问题（甚至报错）
 *
 * 3. 如何保证内存对齐？how
 * - 尽量避免使用 malloc/new，可以用 posix_memalign/aligned_alloc 代替，从而保证内存对齐（手动管理内存）
 * - 可以采用 std::vector 简化内存管理，例如 std::vector<float8_t> a(n)
 * - 通常来讲直接在 stack 上分配的内存不太会出问题 maybe？ malloc/new 一般来说是从 heap 上分配内存
 *
 *
 * 下面的例子是定义 float8_t 指针使用 malloc 申请 heap 内存的例子（编译器比较智能，复现错误的概率不高，不用过于在意这个例子）
 * */

#include <iostream>
#include <cstdlib>
#include <immintrin.h>  // 包含 SIMD 指令集

typedef float float8_t __attribute__ ((vector_size (8 * sizeof(float))));

void crash_unaligned() {
    float8_t a = {1, 2, 3, 4, 5, 6, 7, 8};

    float8_t *p = (float8_t *) malloc(sizeof(float8_t));
    float8_t *q = (float8_t *) malloc(sizeof(float8_t));

    if (!p || !q) {
        std::cerr << "Memory allocation failed\n";
        free(p);
        free(q);
        return;
    }

    std::cout << "Address of p: " << p << "\n";
    std::cout << "Address of q: " << q << "\n";

    // 计算指针之间的字节差异
    intptr_t diff = (intptr_t) q - (intptr_t) p;
    std::cout << "Byte difference between q and p: " << diff << " bytes\n";

    *p = a;
    *q = a;
    a = *p + *q;

    std::cout << "Result: " << a[0] << "\n";

    free(p);
    free(q);
}

void crash_aligned() {
    float8_t a = {1, 2, 3, 4, 5, 6, 7, 8};

    float8_t *p;
    float8_t *q;

    // 使用 posix_memalign 分配 32 字节对齐的内存
    if (posix_memalign((void **) &p, 32, sizeof(float8_t)) != 0) {
        std::cerr << "Memory allocation failed\n";
        return;
    }
    if (posix_memalign((void **) &q, 32, sizeof(float8_t)) != 0) {
        std::cerr << "Memory allocation failed\n";
        free(p);
        return;
    }

    std::cout << "Address of p: " << p << "\n";
    std::cout << "Address of q: " << q << "\n";

    // 计算指针之间的字节差异
    intptr_t diff = (intptr_t) q - (intptr_t) p;
    std::cout << "Byte difference between q and p: " << diff << " bytes\n";

    *p = a;
    *q = a;
    a = *p + *q;

    std::cout << "Result: " << a[0] << "\n";

    free(p);
    free(q);
}

int main() {
    std::cout << "Aligned memory example:\n";
    crash_aligned();

    std::cout << "\nUnaligned memory example:\n";
    crash_unaligned();

    return 0;
}
