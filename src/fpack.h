/*
 * MIT License
 * 
 * Copyright (c) 2017 Matthew T. Bucknall
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef _FPACK_H_
#define _FPACK_H_

#include <stddef.h>
#include <stdint.h>


#define FPK_ENABLE_RESULT_TO_STRING     1
#define FPK_ENABLE_HMAC_SHA256          1
#define FPK_ENABLE_AES128_CBC           1


typedef enum
{
    FPK_RESULT_OK,
    FPK_RESULT_READ_ERROR,
    FPK_RESULT_UNEXPECTED_END_OF_INPUT,
    FPK_RESULT_ERASE_ERROR,
    FPK_RESULT_PROGRAM_ERROR,
    FPK_RESULT_UNKNOWN_ID,
    FPK_RESULT_CRC_MISMATCH,
    FPK_RESULT_INVALID_SIGNATURE,
    FPK_RESULT_NO_AUTHENTICATION_KEY,
    FPK_RESULT_NO_CIPHER_KEY,
    FPK_RESULT_UNSUPPORTED_AUTHENTICATION_TYPE,
    FPK_RESULT_UNSUPPORTED_CIPHER_TYPE,
    FPK_RESULT_UNSUPPORTED_FPK_FILE_VERSION,
    FPK_RESULT_INVALID_FPK_FILE

} fpk_result_t;


typedef enum
{
    FPK_AUTHENTICATION_TYPE_NONE,
    FPK_AUTHENTICATION_TYPE_HMAC_SHA256

} fpk_authentication_type_t;


typedef enum
{
    FPK_CIPHER_TYPE_NONE,
    FPK_CIPHER_TYPE_AES128_CBC

} fpk_cipher_type_t;


typedef struct
{
    fpk_result_t (*read_file) (uint8_t* buffer, uint8_t n_bytes,
            void* user_data);

    fpk_result_t (*seek_file) (uint32_t position, void* user_data);

    fpk_result_t (*prepare_memory) (const char* id, void* user_data);

    fpk_result_t (*program_memory) (const char* id, const uint8_t* data,
            uint8_t length, void* user_data);
    
    fpk_result_t (*finalize_memory) (const char* id, void* user_data);

    const uint8_t* (*authentication_key) (fpk_authentication_type_t type,
            void* user_data);

    const uint8_t* (*cipher_key) (fpk_cipher_type_t type, void* user_data);

} fpk_hooks_t;


typedef struct 
{
    uint32_t options;
    const fpk_hooks_t* hooks;
    void* user_data;
    uint8_t input[16];
    uint8_t flags;
    uint32_t crc32;
    uint32_t n_blocks;
    
#ifdef FPK_ENABLE_HMAC_SHA256
    
    uint8_t sha256_buffer[64];
    uint8_t sha256_m[64];
    uint32_t sha256_state[8];
    uint8_t sha256_buffer_in;
    uint64_t sha256_bit_len;
    
    
#endif /* FPK_ENABLE_HMAC_SHA256 */

#ifdef FPK_ENABLE_AES128_CBC

    uint8_t aes128_round_key[176];
    uint8_t aes128_iv[16];
    
#endif /* FPK_ENABLE_AES128_CBC */
    
} fpk_context_t;


#define FPK_OPTION_ENFORCE_AUTHENTICATION       (1 << 0)


fpk_result_t fpk_unpack(fpk_context_t* ctx, uint32_t options,
        const fpk_hooks_t* hooks, void* user_data);


#ifdef FPK_ENABLE_RESULT_TO_STRING

const char* fpk_result_to_string(fpk_result_t result);

#endif /* FPK_ENABLE_RESULT_TO_STRING */

#endif /* _FPACK_H_ */
