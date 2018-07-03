#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef uint8_t u8;       ///<   8-bit unsigned integer.
typedef uint16_t u16;     ///<  16-bit unsigned integer.
typedef uint32_t u32;     ///<  32-bit unsigned integer.
typedef uint64_t u64;     ///<  64-bit unsigned integer.
typedef __uint128_t u128; ///< 128-bit unsigned integer.

struct ReplacedTitle
{
    u64 title_id;
    char path[50];
};

bool getReplacement(u64 title_id, char* path);
void removeReplacement(u64 title_id);
void listReplacement();
void addReplacement(u64 title_id, char* path);