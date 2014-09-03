#include <tmmintrin.h>

typedef uint8_t v16qu __attribute__((vector_size(16)));
typedef uint16_t v8hu __attribute__((vector_size(16)));
typedef uint32_t v4su __attribute__((vector_size(16)));



/* SSSE3 string -> integer conversion. */
static inline int asciitoi(uint8_t **str_p) {
    static const v16qu mul8[11] = {{0}, {1, 0}, {10, 1}, {10, 1, 1},
        {10, 1, 10, 1}, {10, 1, 10, 1, 1}, {10, 1, 10, 1, 10, 1},
        {10, 1, 10, 1, 10, 1, 1}, {10, 1, 10, 1, 10, 1, 10, 1},
        {10, 1, 10, 1, 10, 1, 10, 1, 1}, {10, 1, 10, 1, 10, 1, 10, 1, 10, 1}};
    static const v8hu mul16[11] = {{0}, {1}, {1}, {10, 1}, {100, 1}, {100, 1, 1},
        {100, 1, 1}, {100, 1, 10, 1}, {100, 1, 100, 1}, {100, 1, 1000, 10, 1},
        {100, 1, 10000, 100, 1}};
    static const uint32_t mul32[11] = {0, 1, 1, 1, 1, 10, 100, 1000, 10000,
        100000, 1000000};
    
    uint8_t *str = *str_p;
    while (*str - 1 < ' ')
        str++;
    int negate = (*str == '-');
    str += negate;
    
    v16qu x0 = _mm_sub_epi8(_mm_lddqu_si128((__m128i *)str), _mm_set1_epi8('0'));
    unsigned int digits = __builtin_ctz(1 << 10 | _mm_movemask_epi8(x0) |
        _mm_movemask_epi8(_mm_cmpgt_epi8(x0, _mm_set1_epi8(9))));
    v8hu x1 = _mm_maddubs_epi16(x0, mul8[digits]);
    v4su x2 = _mm_madd_epi16(x1, mul16[digits]);
    *str_p = str + digits;
    return x2[0] * mul32[digits] + x2[1] + x2[2];
}
