#ifndef STRING_HASH_COMPILE_TIME
#define STRING_HASH_COMPILE_TIME

// hash func and operator overload to allow swich case of string
typedef uint64_t hash_t;
constexpr hash_t prime = 0x100000001B3ull;  
constexpr hash_t basis = 0xCBF29CE484222325ull;  

hash_t hash_(char const* str){
    hash_t ret{basis};
    while(*str){
        ret ^= *str;
        ret *= prime;
        str++;
    }
    return ret;
}

constexpr hash_t hash_compile_time(char const *str,hash_t last_value = basis){
    return *str? hash_compile_time(str+1,(*str ^ last_value) * prime) : last_value;
}
constexpr unsigned long long  operator "" _hash(char const * p ,size_t){
    return hash_compile_time(p);
}


#endif /* STRING_HASH_COMPILE_TIME */
