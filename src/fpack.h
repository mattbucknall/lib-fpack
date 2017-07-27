/*
 * Copyright 2017 Matthew T. Bucknall
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
    FPK_RESULT_NO_CIPHER_KEY,
    FPK_RESULT_UNSUPPORTED_AUTHENTICATION_TYPE,
    FPK_RESULT_UNSUPPORTED_CIPHER_TYPE

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
    uint8_t buffer[16];
    uint8_t buffer_in;
    uint8_t buffer_out;
    
#ifdef FPK_ENABLE_HMAC_SHA256
    
    uint8_t hmac[64];
    uint8_t sha256_data[64];
    uint8_t sha256_data_len;
    uint32_t sha256_bit_len;
    uint32_t sha256_state;
    
#endif /* FPK_ENABLE_HMAC_SHA256 */
    
} fpk_context_t;


#define FPK_OPTION_ENFORCE_AUTHENTICATION       (1 << 0)


fpk_result_t fpk_unpack(fpk_context_t* ctx, uint32_t options,
        const fpk_hooks_t* hooks, void* user_data);


#ifdef FPK_ENABLE_RESULT_TO_STRING

const char* fpk_result_to_string(fpk_result_t result);

#endif /* FPK_ENABLE_RESULT_TO_STRING */

#endif /* _FPACK_H_ */
