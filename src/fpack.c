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

#include "fpack.h"


#ifdef FPK_ENABLE_RESULT_TO_STRING

const char* fpk_result_to_string(fpk_result_t result)
{
    switch(result)
    {
    case FPK_RESULT_OK:
        return "ok";

    case FPK_RESULT_READ_ERROR:
        return "read error";

    case FPK_RESULT_UNEXPECTED_END_OF_INPUT:
        return "unexpected end of input";

    case FPK_RESULT_ERASE_ERROR:
        return "erase error";

    case FPK_RESULT_PROGRAM_ERROR:
        return "program error";

    case FPK_RESULT_UNKNOWN_ID:
        return "unknown id";

    case FPK_RESULT_CRC_MISMATCH:
        return "crc mismatch";

    case FPK_RESULT_INVALID_SIGNATURE:
        return "invalid signature";

    case FPK_RESULT_NO_CIPHER_KEY:
        return "no cipher key";

    case FPK_RESULT_UNSUPPORTED_AUTHENTICATION_TYPE:
        return "unsupported authentication type";

    case FPK_RESULT_UNSUPPORTED_CIPHER_TYPE:
        return "unsupported cipher type";
        
    case FPK_RESULT_UNSUPPORTED_VERSION:
        return "unsupported fpk file version";
        
    case FPK_RESULT_INVALID_FPK_FILE:
        return "invalid fpk file";
        
    default:
        return "undefined result";
    }
}

#endif /* FPK_ENABLE_RESULT_TO_STRING */


static fpk_result_t read_file(fpk_context_t* ctx, uint8_t* buffer,
        uint8_t n_bytes)
{
    fpk_result_t result;
    
    result = ctx->hooks->read_file(buffer, n_bytes, ctx->user_data);
    
    if ( result == FPK_RESULT_OK )
    {
    }
    
    return result;
}


static fpk_result_t seek_file(fpk_context_t* ctx, uint32_t position)
{
    return ctx->hooks->seek_file(position, ctx->user_data);
}


static fpk_result_t verify_preamble(fpk_context_t* ctx)
{
    
}


fpk_result_t fpk_unpack(fpk_context_t* ctx, uint32_t options,
        const fpk_hooks_t* hooks, void* user_data)
{
    fpk_result_t result;
    
    ctx->options = options;
    ctx->hooks = hooks;
    ctx->user_data = user_data;
    ctx->buffer_in = 0;
    ctx->buffer_out = 0;
    
    result = verify_preamble(ctx);
    if ( result != FPK_RESULT_OK ) return result;
    
    return FPK_RESULT_OK;
}
