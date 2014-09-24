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
    return ((x2[0] * mul32[digits] + x2[1] + x2[2]) ^ -negate) + negate;
}



/* SSE2 integer -> string conversion. */
uint8_t* itostr(uint8_t* str, int i) {
    static const uint16_t digits[100] = {
        12336, 12592, 12848, 13104, 13360, 13616, 13872, 14128, 14384, 14640,
        12337, 12593, 12849, 13105, 13361, 13617, 13873, 14129, 14385, 14641,
        12338, 12594, 12850, 13106, 13362, 13618, 13874, 14130, 14386, 14642,
        12339, 12595, 12851, 13107, 13363, 13619, 13875, 14131, 14387, 14643,
        12340, 12596, 12852, 13108, 13364, 13620, 13876, 14132, 14388, 14644,
        12341, 12597, 12853, 13109, 13365, 13621, 13877, 14133, 14389, 14645,
        12342, 12598, 12854, 13110, 13366, 13622, 13878, 14134, 14390, 14646,
        12343, 12599, 12855, 13111, 13367, 13623, 13879, 14135, 14391, 14647,
        12344, 12600, 12856, 13112, 13368, 13624, 13880, 14136, 14392, 14648,
        12345, 12601, 12857, 13113, 13369, 13625, 13881, 14137, 14393, 14649,
    };
    
    unsigned int u = i;
    if (i < 0)
        u = -i, *str++ = '-';
    
    __m128i x = _mm_setzero_si128();
    uint8_t *end = str + 2;
    while (u >= 100) {
        x = _mm_insert_epi16(x, digits[u % 100], 0);
        u /= 100;
        x = _mm_slli_si128(x, 2);
        end += 2;
    }
    x = _mm_insert_epi16(x, digits[u], 0);
    if (u < 10)
        end--, x = _mm_srli_si128(x, 1);
    _mm_storeu_ps((float *)str, x);
    return end;
}
