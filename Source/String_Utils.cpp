#include <cstddef>
#include <cstring>
#include "../Include/String_Utils.hpp"
#include "../Include/Memory_Utils.hpp"

using namespace Utils;

char *Utils::String::AllocateAndCopyString(const char *string) {
    if (string == NULL) return NULL;
    size_t str_length = strlen(string);
    char *res = Memory::malloc<char>(str_length + 1);
    memcpy(res, string, str_length);
    res[str_length] = '\0';
    return res;
}

bool Utils::String::StringToInt64(char *string, int64_t *value_out, int base) {
    char *valid;
    *value_out = strtoll(string, &valid, base);
    return *valid == '\0';
}