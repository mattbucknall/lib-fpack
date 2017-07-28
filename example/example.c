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

#include <stdio.h>

#include "fpack.h"


static fpk_context_t m_ctx;
static FILE* m_input;
static FILE* m_output;


static fpk_result_t read_file_cb(uint8_t* buffer, uint8_t n_bytes,
            void* user_data)
{
    return fread(buffer, n_bytes, 1, m_input) == n_bytes ?
            FPK_RESULT_OK : FPK_RESULT_READ_ERROR;
}


static fpk_result_t seek_file_cb(uint32_t position, void* user_data)
{
    return FPK_RESULT_OK;
}


static fpk_result_t prepare_memory_cb(const char* id, void* user_data)
{
    m_output = fopen(id, "wb");
    
    if ( m_output ) return FPK_RESULT_OK;
    else return FPK_RESULT_PROGRAM_ERROR;
}


static fpk_result_t program_memory_cb(const char* id, const uint8_t* data,
        uint8_t length, void* user_data)
{
    size_t n_written = fwrite(data, length, 1, m_output);
    
    if ( n_written == length ) return FPK_RESULT_OK;
    else return FPK_RESULT_PROGRAM_ERROR;
}


static fpk_result_t finalize_memory_cb(const char* id, void* user_data)
{
    fclose(m_output);
    return FPK_RESULT_OK;
}


static const uint8_t* authentication_key_cb(fpk_authentication_type_t type,
        void* user_data)
{
    static const uint8_t KEY[] = {
        0x4D, 0x6F, 0x1F, 0xFE, 0xAE, 0xB9, 0xA7, 0xDD,
        0x1E, 0xCB, 0x19, 0xDE, 0x03, 0x86, 0xB1, 0x53,
        0xCC, 0x21, 0x01, 0x64, 0x79, 0xEF, 0x55, 0x7B,
        0x8E, 0x60, 0x51, 0x20, 0x3B, 0x4E, 0x3D, 0x25, 
    };
    
    if ( type == FPK_AUTHENTICATION_TYPE_HMAC_SHA256 ) return KEY;
    else return NULL;
}


static const uint8_t* cipher_key_cb(fpk_cipher_type_t type, void* user_data)
{
    static const uint8_t KEY[] = {
        0x31, 0x37, 0xA6, 0x15, 0xBD, 0xA5, 0x2F, 0x05,
        0xA6, 0x79, 0x24, 0xBF, 0x53, 0x4E, 0x8C, 0x1B
    };
    
    if ( type == FPK_CIPHER_TYPE_AES128_CBC ) return KEY;
    else return NULL;
}


static const fpk_hooks_t m_hooks =
{
    .read_file =            read_file_cb,
    .seek_file =            seek_file_cb,
    .prepare_memory =       prepare_memory_cb,
    .program_memory =       program_memory_cb,
    .finalize_memory =      finalize_memory_cb,
    .authentication_key =   authentication_key_cb,
    .cipher_key =           cipher_key_cb
};


int main(int argc, char* argv[])
{
    fpk_result_t result;
    
    printf("%lu\n", sizeof(fpk_context_t));
    
    if ( argc < 2 )
    {
        puts("Usage: example <fpk-file>");
        return 0;
    }
    
    m_input = fopen(argv[1], "rb");
    
    if ( !m_input )
    {
        fprintf(stderr, "Fatal error: Unable to open file: %s\n", argv[1]);
        return 1;
    }
    
    result = fpk_unpack(
        &m_ctx,
        FPK_OPTION_ENFORCE_AUTHENTICATION,
        &m_hooks,
        NULL
    );
    
    fclose(m_input);
    
    if ( result == FPK_RESULT_OK )
    {
        return 0;
    }
    else
    {
        fprintf(stderr, "Fatal error: %s\n", fpk_result_to_string(result));
        return 1;
    }
}
