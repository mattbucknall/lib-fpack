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
#include <stdio.h>
#include <string.h>

#include "fpack.h"


/* ==== CRC32 ============================================================== */

// CRC32 code based on implementation by Gary S. Brown

static const uint32_t CRC32_LUT[] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


static void crc32_reset(fpk_context_t* ctx)
{
    ctx->crc32 = 0xFFFFFFFFUL;
}


static void crc32_update(fpk_context_t* ctx, const uint8_t* data,
        uint32_t length)
{
    const uint8_t* i = data;
    const uint8_t* e = data + length;
    uint32_t crc = ~ctx->crc32;
    
    while (i != e)
    {
        crc = CRC32_LUT[(crc ^ *i++) & 0xFF] ^ (crc >> 8);
    }
    
    ctx->crc32 = ~crc;
}


/* ==== HMAC-SHA256 ======================================================== */

#ifdef FPK_ENABLE_HMAC_SHA256

// SHA256 code based on implementation by Brad Conte

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))


static const uint32_t k[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


static void sha256_reset(fpk_context_t* ctx)
{
    uint32_t* state = ctx->sha256_state;
    
    ctx->sha256_buffer_in = 0;
    ctx->sha256_bit_len = 0;
    
    state[0] = 0x6a09e667;
    state[1] = 0xbb67ae85;
    state[2] = 0x3c6ef372;
    state[3] = 0xa54ff53a;
    state[4] = 0x510e527f;
    state[5] = 0x9b05688c;
    state[6] = 0x1f83d9ab;
    state[7] = 0x5be0cd19;
}


static void sha256_transform(fpk_context_t* ctx)
{
    uint32_t* state = ctx->sha256_state;
    uint32_t* m = ctx->sha256_m;
    uint8_t* buffer = ctx->sha256_buffer;
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2;
    
    for (i = 0, j = 0; i < 16; ++i, j += 4)
    {
        m[i] = (buffer[j] << 24) | (buffer[j + 1] << 16) |
                (buffer[j + 2] << 8) | (buffer[j + 3]);
    }
    
    for (; i < 64; ++i)
    {
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    }

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];

    for (i = 0; i < 64; ++i)
    {
        t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}


static void sha256_update(fpk_context_t* ctx, const uint8_t* data,
        uint32_t length)
{
    uint8_t* buffer = ctx->sha256_buffer;
    const uint8_t* i = data;
    const uint8_t* e = data + length;
    
    while (i != e)
    {
        buffer[ctx->sha256_buffer_in++] = *i++;
        
        if ( ctx->sha256_buffer_in >= 64 )
        {
            sha256_transform(ctx);
            ctx->sha256_bit_len += 512;
            ctx->sha256_buffer_in = 0;
        }
    }
}


static void sha256_digest(fpk_context_t* ctx, uint8_t* hash)
{
    uint8_t* buffer = ctx->sha256_buffer;
    uint32_t in = ctx->sha256_buffer_in;
    uint64_t bit_len = ctx->sha256_bit_len;
    uint32_t* state = ctx->sha256_state;
    
    if ( in < 56 )
    {
        buffer[in++] = 0x80;
        while (in < 56) buffer[in++] = 0x00;
    }
    else if ( in < 64 )
    {
        buffer[in++] = 0x80;
        while (in < 64) buffer[in++] = 0x00;
        sha256_transform(ctx);
        
        for (in = 0; in < 56; in++) buffer[in] = 0x00;
    }
    
    bit_len += ctx->sha256_buffer_in * 8;
    
    buffer[63] = bit_len;
    buffer[62] = bit_len >> 8;
    buffer[61] = bit_len >> 16;
    buffer[60] = bit_len >> 24;
    buffer[59] = bit_len >> 32;
    buffer[58] = bit_len >> 40;
    buffer[57] = bit_len >> 48;
    buffer[56] = bit_len >> 56;
    
    sha256_transform(ctx);
    
    for (uint8_t i = 0; i < 4; ++i)
    {
        hash[i]         = (state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4]     = (state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8]     = (state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12]    = (state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16]    = (state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20]    = (state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24]    = (state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28]    = (state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}


static void dump_hex(const uint8_t* buffer, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x", buffer[i]);
    }

    printf("\n");
}


static void hmac_reset(fpk_context_t* ctx, const uint8_t* key)
{
    uint8_t i_key[8];
    
    sha256_reset(ctx);
    
    for (uint8_t i = 0; i < 32; i += 8)
    {
        i_key[0] = key[i] ^ 0x36;
        i_key[1] = key[i + 1] ^ 0x36;
        i_key[2] = key[i + 2] ^ 0x36;
        i_key[3] = key[i + 3] ^ 0x36;
        i_key[4] = key[i + 4] ^ 0x36;
        i_key[5] = key[i + 5] ^ 0x36;
        i_key[6] = key[i + 6] ^ 0x36;
        i_key[7] = key[i + 7] ^ 0x36;
        
        sha256_update(ctx, i_key, 8);
    }
    
    memset(i_key, 0x36, 8);
    
    for (uint8_t i = 0; i < 4; i++)
    {
        sha256_update(ctx, i_key, 8);
    }
}


static void hmac_update(fpk_context_t* ctx, const uint8_t* data,
        uint32_t length)
{
    sha256_update(ctx, data, length);
}


static void hmac_digest(fpk_context_t* ctx, const uint8_t* key, uint8_t* hash)
{
    uint8_t o_key[8];
    
    sha256_digest(ctx, hash);
    sha256_reset(ctx);
    
    for (uint8_t i = 0; i < 32; i += 8)
    {
        o_key[0] = key[i] ^ 0x5C;
        o_key[1] = key[i + 1] ^ 0x5C;
        o_key[2] = key[i + 2] ^ 0x5C;
        o_key[3] = key[i + 3] ^ 0x5C;
        o_key[4] = key[i + 4] ^ 0x5C;
        o_key[5] = key[i + 5] ^ 0x5C;
        o_key[6] = key[i + 6] ^ 0x5C;
        o_key[7] = key[i + 7] ^ 0x5C;
        
        sha256_update(ctx, o_key, 8);
    }
    
    memset(o_key, 0x5C, 8);
    
    for (uint8_t i = 0; i < 4; i++)
    {
        sha256_update(ctx, o_key, 8);
    }
    
    sha256_update(ctx, hash, 32);
    sha256_digest(ctx, hash);
}

#endif /* FPK_ENABLE_HMAC_SHA256 */


/* ==== AES128-CBC========================================================== */

#ifdef FPK_ENABLE_AES128_CBC

typedef uint8_t aes128_map_t[4][4];


static const uint8_t AES128_SBOX[256] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b,
    0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26,
    0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2,
    0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed,
    0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f,
    0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec,
    0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14,
    0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d,
    0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f,
    0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11,
    0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f,
    0xb0, 0x54, 0xbb, 0x16
};


static const uint8_t AES128_RSBOX[256] =
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e,
    0x81, 0xf3, 0xd7, 0xfb, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32,
    0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49,
    0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50,
    0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05,
    0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41,
    0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8,
    0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b,
    0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59,
    0x27, 0x80, 0xec, 0x5f, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0, 0xe0, 0x3b, 0x4d,
    0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63,
    0x55, 0x21, 0x0c, 0x7d
};


static const uint8_t AES128_RCON[11] =
{
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};


static void aes128_add_round_key(fpk_context_t* ctx, uint8_t round,
        uint8_t* block)
{
    const uint8_t* round_key = ctx->aes128_round_key;
    
    round *= 16;
    
    for (uint8_t i = 0; i < 16; i++)
    {
        block[i] ^= round_key[round + i];
    }
}


static void aes128_inv_shift_rows(fpk_context_t* ctx, uint8_t* block)
{
    aes128_map_t* map = (aes128_map_t*) block;
    uint8_t byte;
    
    byte = *map[3][1];
    *map[3][1] = *map[2][1];
    *map[2][1] = *map[1][1];
    *map[1][1] = *map[0][1];
    *map[0][1] = byte;
    
    byte = *map[0][2];
    *map[0][2] = *map[2][2];
    *map[2][2] = byte;
    
    byte = *map[1][2];
    *map[1][2] = *map[3][2];
    *map[3][2] = byte;
    
    byte = *map[0][3];
    *map[0][3] = *map[1][3];
    *map[1][3] = *map[2][3];
    *map[2][3] = *map[3][3];
    *map[3][3] = byte;
}


static void aes128_inv_substitute(fpk_context_t* ctx, uint8_t* block)
{
    for (uint8_t i = 0; i < 16; i++)
    {
        block[i] = AES128_RSBOX[ block[i] ];
    }
}


static uint8_t aes128_x_time(uint8_t x)
{
    return (x << 1) ^ ( ( (x >> 7) & 1) * 0x1b);
}


static uint8_t aes128_multiply(uint8_t x, uint8_t y)
{
    return (((y & 1) * x) ^
        ((y>>1 & 1) * aes128_x_time(x)) ^
        ((y>>2 & 1) * aes128_x_time(aes128_x_time(x))) ^
        ((y>>3 & 1) * aes128_x_time(aes128_x_time(aes128_x_time(x)))) ^
        ((y>>4 & 1) *
        aes128_x_time(aes128_x_time(aes128_x_time(aes128_x_time(x))))));
}


static void aes128_inv_mix_columns(fpk_context_t* ctx, uint8_t* block)
{
    aes128_map_t* map = (aes128_map_t*) block;
    
    for (uint8_t i = 0; i < 4; i++)
    {
        uint8_t a = *map[i][0];
        uint8_t b = *map[i][1];
        uint8_t c = *map[i][2];
        uint8_t d = *map[i][3];
        
        *map[i][0] = aes128_multiply(a, 0x0e) ^ aes128_multiply(b, 0x0b) ^
                aes128_multiply(c, 0x0d) ^ aes128_multiply(d, 0x09);
        
        *map[i][1] = aes128_multiply(a, 0x09) ^ aes128_multiply(b, 0x0e) ^
                aes128_multiply(c, 0x0b) ^ aes128_multiply(d, 0x0d);
        
        *map[i][2] = aes128_multiply(a, 0x0d) ^ aes128_multiply(b, 0x09) ^
                aes128_multiply(c, 0x0e) ^ aes128_multiply(d, 0x0b);
        
        *map[i][3] = aes128_multiply(a, 0x0b) ^ aes128_multiply(b, 0x0d) ^
                aes128_multiply(c, 0x09) ^ aes128_multiply(d, 0x0e);
    }
}


static void aes128_init(fpk_context_t* ctx, const uint8_t* key, const uint8_t* iv)
{
    uint8_t* round_key = ctx->aes128_round_key;
    uint8_t temp[4];
    uint8_t byte;
    
    memcpy(ctx->aes128_iv, iv, 16);
    memcpy(round_key, key, 16);
    
    for (uint8_t i = 4; i < 44; i++)
    {
        uint8_t p = (i - 1) * 4;
        
        temp[0] = round_key[p];
        temp[1] = round_key[p + 1];
        temp[2] = round_key[p + 2];
        temp[3] = round_key[p + 4];
        
        if ( !(i & 3) )
        {
            byte = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = byte;
            
            temp[0] = AES128_SBOX[temp[0]];
            temp[1] = AES128_SBOX[temp[1]];
            temp[2] = AES128_SBOX[temp[2]];
            temp[3] = AES128_SBOX[temp[3]];
            
            temp[0] = temp[0] ^ AES128_RCON[i / 4];
        }
        
        p = (i - 4) * 4;
        
        round_key[i * 4] = round_key[p] ^ temp[0];
        round_key[i * 4 + 1] = round_key[p + 1] ^ temp[1];
        round_key[i * 4 + 2] = round_key[p + 2] ^ temp[2];
        round_key[i * 4 + 3] = round_key[p + 3] ^ temp[3];
    }
}


static void aes128_decrypt_block(fpk_context_t* ctx, uint8_t* block)
{
    aes128_add_round_key(ctx, 10, block);
    
    for (uint8_t round = 9; round > 0; round--)
    {
        aes128_inv_shift_rows(ctx, block);
        aes128_inv_substitute(ctx, block);
        aes128_add_round_key(ctx, round, block);
        aes128_inv_mix_columns(ctx, block);
    }
    
    aes128_inv_shift_rows(ctx, block);
    aes128_inv_substitute(ctx, block);
    aes128_add_round_key(ctx, 0, block);
}


static void aes128_decrypt_cbc(fpk_context_t* ctx, uint8_t* block)
{
    uint8_t temp[16];
    uint8_t* iv = ctx->aes128_iv;
    
    memcpy(temp, iv, 16);
    memcpy(iv, block, 16);
    
    aes128_decrypt_block(ctx, block);
    
    for (uint8_t i = 0; i < 16; i++)
    {
        block[i] ^= temp[i];
    }
}

#endif /* FPK_ENABLE_AES128_CBC */


/* ==== HOOK WRAPPERS ====================================================== */

static fpk_result_t read_file(fpk_context_t* ctx, uint8_t* buffer,
        uint8_t n_bytes)
{
    if ( !ctx->hooks->read_file ) return FPK_RESULT_MANDATORY_HOOK_MISSING;
    return ctx->hooks->read_file(buffer, n_bytes, ctx->user_data);
}


static fpk_result_t seek_file(fpk_context_t* ctx, uint32_t position)
{
    if ( !ctx->hooks->seek_file ) return FPK_RESULT_MANDATORY_HOOK_MISSING;
    return ctx->hooks->seek_file(position, ctx->user_data);
}


static fpk_result_t prepare_memory(fpk_context_t* ctx, const char* id,
        uint32_t size)
{
    if ( !ctx->hooks->prepare_memory ) return FPK_RESULT_OK;
    return ctx->hooks->prepare_memory(id, size, ctx->user_data);
}


static fpk_result_t program_memory(fpk_context_t* ctx, const char* id,
        const uint8_t* data, uint8_t length)
{
    if ( !ctx->hooks->program_memory ) return FPK_RESULT_PROGRAM_ERROR;
    return ctx->hooks->program_memory(id, data, length, ctx->user_data);
}


static fpk_result_t finalize_memory(fpk_context_t* ctx, const char* id)
{
    if ( !ctx->hooks->finalize_memory ) return FPK_RESULT_OK;
    return ctx->hooks->finalize_memory(id, ctx->user_data);
}


#ifdef FPK_ENABLE_HMAC_SHA256

static const uint8_t* authentication_key(fpk_context_t* ctx,
        fpk_authentication_type_t type)
{
    if ( !ctx->hooks->authentication_key ) return NULL;
    return ctx->hooks->authentication_key(type, ctx->user_data);
}

#endif /* FPK_ENABLE_HMAC_SHA256 */


#ifdef FPK_ENABLE_AES128_CBC

static const uint8_t* cipher_key(fpk_context_t* ctx, fpk_cipher_type_t type)
{
    if ( !ctx->hooks->cipher_key ) return NULL;
    return ctx->hooks->cipher_key(type, ctx->user_data);
}

#endif /* FPK_ENABLE_AES128_CBC */


static fpk_result_t handle_metadata(fpk_context_t* ctx, const char* key,
        const char* value)
{
    return ctx->hooks->handle_metadata(key, value, ctx->user_data);
}


/* ==== INPUT PARSING ====================================================== */

#define FLAG_CAPTURE_CRC32      (1 << 0)
#define FLAG_CAPTURE_AUTH       (1 << 1)
#define FLAG_DECIPHER           (1 << 2)


static fpk_result_t read_block(fpk_context_t* ctx)
{
    fpk_result_t result;
    uint8_t flags = ctx->flags;

    result = read_file(ctx, ctx->input, 16);
    if ( result != FPK_RESULT_OK ) return result;

    if ( flags & FLAG_CAPTURE_CRC32 ) crc32_update(ctx, ctx->input, 16);

#ifdef FPK_ENABLE_HMAC_SHA256
    if ( flags & FLAG_CAPTURE_AUTH ) hmac_update(ctx, ctx->input, 16);
#endif /* FPK_ENABLE_HMAC_SHA256 */

#ifdef FPK_ENABLE_AES128_CBC
    if ( flags & FLAG_DECIPHER ) aes128_decrypt_cbc(ctx, ctx->input);
#endif /* FPK_ENABLE_AES128_CBC */

    return result;
}


static fpk_result_t read_input(fpk_context_t* ctx, uint8_t* buffer,
        uint32_t length)
{
    uint8_t* input = ctx->input;
    uint8_t* i = buffer;
    uint8_t* e = buffer + length;
    
    while (i != e)
    {
        if ( ctx->cursor == 0 )
        {
            fpk_result_t result;
            
            if ( ctx->n_blocks == 0 )
                return FPK_RESULT_UNEXPECTED_END_OF_INPUT;
            
            result = read_block(ctx);
            if ( result != FPK_RESULT_OK ) return result;
            
            ctx->n_blocks--;
        }
        
        *i++ = input[ctx->cursor];
        
        ctx->cursor = (ctx->cursor + 1) & 15;
    }
    
    return FPK_RESULT_OK;
}


static uint16_t parse_u16(const uint8_t* buffer)
{
    return buffer[0] | (buffer[1] << 8);
}


static uint32_t parse_u32(const uint8_t* buffer)
{
    return (uint32_t) buffer[0] | ( (uint32_t) buffer[1] << 8) |
            ( (uint32_t) buffer[2] << 16) | ( (uint32_t) buffer[3] << 24);
}


/* ==== API ================================================================ */


#ifdef FPK_ENABLE_RESULT_TO_STRING

const char* fpk_result_to_string(fpk_result_t result)
{
    switch(result)
    {
    case FPK_RESULT_OK:
        return "OK";

    case FPK_RESULT_READ_ERROR:
        return "Read error";

    case FPK_RESULT_UNEXPECTED_END_OF_INPUT:
        return "Unexpected end of input";

    case FPK_RESULT_ERASE_ERROR:
        return "Erase error";

    case FPK_RESULT_PROGRAM_ERROR:
        return "Program error";

    case FPK_RESULT_UNKNOWN_ID:
        return "Unknown id";

    case FPK_RESULT_CRC_MISMATCH:
        return "CRC mismatch";

    case FPK_RESULT_INVALID_SIGNATURE:
        return "Invalid signature";
        
    case FPK_RESULT_SIGNATURE_MISSING:
        return "Signature missing";

    case FPK_RESULT_NO_AUTHENTICATION_KEY:
        return "No authentication key";

    case FPK_RESULT_NO_CIPHER_KEY:
        return "No cipher key";

    case FPK_RESULT_UNSUPPORTED_AUTHENTICATION_TYPE:
        return "Unsupported authentication type";

    case FPK_RESULT_UNSUPPORTED_CIPHER_TYPE:
        return "Unsupported cipher type";
        
    case FPK_RESULT_UNSUPPORTED_FPK_FILE_VERSION:
        return "Unsupported FPK file version";
        
    case FPK_RESULT_INVALID_FPK_FILE:
        return "Invalid FPK file";
        
    case FPK_RESULT_INVALID_METADATA:
        return "Invalid metadata";
        
    case FPK_RESULT_INVALID_IMAGE:
        return "Invalid image";
        
    case FPK_RESULT_IMAGE_TOO_LARGE:
        return "Image too large";
        
    case FPK_RESULT_MANDATORY_HOOK_MISSING:
        return "Mandatory hook missing";
        
    default:
        return "Undefined result";
    }
}

#endif /* FPK_ENABLE_RESULT_TO_STRING */


fpk_result_t fpk_unpack(fpk_context_t* ctx, uint32_t options,
        const fpk_hooks_t* hooks, void* user_data)
{
    fpk_result_t result;
    uint8_t* input = ctx->input;
    uint8_t auth_type;
    uint8_t cipher_type;
    uint16_t n_objects;
    uint8_t* key_buffer = ctx->key_buffer;
    uint8_t* data_buffer = ctx->data_buffer;
    
#if defined(FPK_ENABLE_HMAC_SHA256) || defined(FPK_ENABLE_AES128_CBC)
    const uint8_t* key = NULL;
#endif
    
    ctx->options = options;
    ctx->hooks = hooks;
    ctx->user_data = user_data;
    ctx->cursor = 0;
    ctx->flags = FLAG_CAPTURE_CRC32;

    crc32_reset(ctx);

    result = read_block(ctx);
    if ( result != FPK_RESULT_OK ) return result;

    if ( input[0] != 0x46 ||
        input[1] != 0x50 ||
        input[2] != 0x4B ) return FPK_RESULT_INVALID_FPK_FILE;

    if ( input[3] != 0x00 ) return FPK_RESULT_UNSUPPORTED_FPK_FILE_VERSION;

    ctx->n_blocks = parse_u32(input + 4);

    auth_type = input[8];
    cipher_type = input[9];

#ifdef FPK_ENABLE_HMAC_SHA256

    if ( auth_type == FPK_AUTHENTICATION_TYPE_NONE )
    {
        if ( ctx->options & FPK_OPTION_ENFORCE_AUTHENTICATION )
        {
            return FPK_RESULT_SIGNATURE_MISSING;
        }
    }
    else if ( auth_type == FPK_AUTHENTICATION_TYPE_HMAC_SHA256 )
    {
        key = authentication_key(ctx, auth_type);
        if ( !key ) return FPK_RESULT_NO_AUTHENTICATION_KEY;

        hmac_reset(ctx, key);

        ctx->flags |= FLAG_CAPTURE_AUTH;
    }
    else
    {
        return FPK_RESULT_UNSUPPORTED_AUTHENTICATION_TYPE;
    }

#else /* FPK_ENABLE_HMAC_SHA256 */

    if ( auth_type != FPK_AUTHENTICATION_TYPE_NONE )
    {
        return FPK_RESULT_UNSUPPORTED_AUTHENTICATION_TYPE;
    }

#endif /* FPK_ENABLE_HMAC_SHA256 */

#ifdef FPK_ENABLE_AES128_CBC

    if ( cipher_type != FPK_CIPHER_TYPE_NONE &&
        cipher_type != FPK_CIPHER_TYPE_AES128_CBC )
    {
        return FPK_RESULT_UNSUPPORTED_CIPHER_TYPE;
    }

#else /* FPK_ENABLE_AES128_CBC */

    if ( cipher_type != FPK_CIPHER_TYPE_NONE )
    {
        return FPK_RESULT_UNSUPPORTED_CIPHER_TYPE;
    }

#endif /* FPK_ENABLE_AES128_CBC */

    for (uint32_t i = ctx->n_blocks; i--;)
    {
        result = read_block(ctx);
        if ( result != FPK_RESULT_OK ) return result;
    }
    
    ctx->flags &= ~FLAG_CAPTURE_AUTH;

#ifdef FPK_ENABLE_HMAC_SHA256

    if ( auth_type == FPK_AUTHENTICATION_TYPE_HMAC_SHA256 )
    {
        hmac_digest(ctx, key, ctx->hmac);
        
        result = read_block(ctx);
        if ( result != FPK_RESULT_OK ) return result;

        if ( memcmp(input, ctx->hmac, 16) != 0 )
            return FPK_RESULT_INVALID_SIGNATURE;

        result = read_block(ctx);
        if ( result != FPK_RESULT_OK ) return result;
        
        if ( memcmp(input, ctx->hmac + 16, 16) != 0 )
            return FPK_RESULT_INVALID_SIGNATURE;
    }

#endif /* FPK_ENABLE_HMAC_SHA256 */

    ctx->flags &= ~FLAG_CAPTURE_CRC32;

    result = read_block(ctx);
    if ( result != FPK_RESULT_OK ) return result;
    
    if ( parse_u32(input) != ctx->crc32 )
        return FPK_RESULT_CRC_MISMATCH;

    result = seek_file(ctx, 16);
    if ( result != FPK_RESULT_OK ) return result;

#ifdef FPK_ENABLE_AES128_CBC

    if ( cipher_type == FPK_CIPHER_TYPE_AES128_CBC )
    {
        key = cipher_key(ctx, cipher_type);
        if ( !key ) return FPK_RESULT_NO_CIPHER_KEY;

        result = read_block(ctx);
        if ( result != FPK_RESULT_OK ) return result;

        aes128_init(ctx, key, input);
        dump_hex(input, 16);

        ctx->flags |= FLAG_DECIPHER;
        ctx->n_blocks--;
    }

#endif /* FPK_ENABLE_AES128_CBC */

    result = read_input(ctx, data_buffer, 2);
    if ( result != FPK_RESULT_OK ) return result;
    
    n_objects = parse_u16(data_buffer);
    
    
    for (uint8_t i = 0; i < n_objects; i++)
    {
        uint8_t key_length;
        uint8_t value_length;
        
        result = read_input(ctx, key_buffer, 1);
        if ( result != FPK_RESULT_OK ) return result;
        
        key_length = key_buffer[0];
        if ( key_length >= FPK_KEY_BUFFER_SIZE )
            return FPK_RESULT_INVALID_METADATA;
        
        result = read_input(ctx, key_buffer, key_length);
        if ( result != FPK_RESULT_OK ) return result;
        
        key_buffer[key_length] = 0;
        
        result = read_input(ctx, data_buffer, 1);
        if ( result != FPK_RESULT_OK ) return result;
        
        value_length = data_buffer[0];
        if ( value_length >= FPK_DATA_BUFFER_SIZE )
            return FPK_RESULT_INVALID_METADATA;
        
        result = read_input(ctx, data_buffer, value_length);
        if ( result != FPK_RESULT_OK ) return result;
        
        data_buffer[value_length] = 0;
        
        result = handle_metadata(
            ctx,
            (const char*) key_buffer,
            (const char*) data_buffer
        );
        
        if ( result != FPK_RESULT_OK ) return result;
    }
    
    result = read_input(ctx, data_buffer, 2);
    if ( result != FPK_RESULT_OK ) return result;
    
    n_objects = parse_u16(data_buffer);
    
    for (uint16_t i = 0; i < n_objects; i++)
    {
        uint8_t id_length;
        uint32_t image_length;
        
        result = read_input(ctx, key_buffer, 1);
        if ( result != FPK_RESULT_OK ) return result;
        
        id_length = key_buffer[0];
        if ( id_length >= FPK_KEY_BUFFER_SIZE )
            return FPK_RESULT_INVALID_IMAGE;
        
        result = read_input(ctx, key_buffer, id_length);
        if ( result != FPK_RESULT_OK ) return result;
        
        key_buffer[id_length] = 0;
        
        result = read_input(ctx, data_buffer, 4);
        if ( result != FPK_RESULT_OK ) return result;
        
        image_length = parse_u32(data_buffer);
        
        result = prepare_memory(
            ctx,
            (const char*) key_buffer,
            image_length
        );
        
        if ( result != FPK_RESULT_OK ) return result;
        
        while (image_length > 0)
        {
            uint32_t remaining = image_length;
            
            if ( remaining > FPK_DATA_BUFFER_SIZE )
                remaining = FPK_DATA_BUFFER_SIZE;
            
            result = read_input(ctx, data_buffer, remaining);
            if ( result != FPK_RESULT_OK ) return result;
            
            result = program_memory(
                ctx,
                (const char*) key_buffer,
                data_buffer,
                remaining
            );
            
            if ( result != FPK_RESULT_OK ) return result;
            
            image_length -= remaining;
        }
        
        result = finalize_memory(
            ctx,
            (const char*) key_buffer
        );
        
        if ( result != FPK_RESULT_OK ) return result;
    }

    return FPK_RESULT_OK;
}
