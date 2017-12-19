#include "../../../include/fxcrt/fx_basic.h"
#include "../../../include/fdrm/fx_crypt.h"
#ifdef __cplusplus
extern "C" {
#endif
struct rc4_state {
    int x, y, m[256];
};
void CRYPT_ArcFourSetup(void* context,  FX_LPCBYTE key,  FX_DWORD length )
{
    rc4_state *s = (rc4_state*)context;
    int i, j, k, *m;
    s->x = 0;
    s->y = 0;
    m = s->m;
    for( i = 0; i < 256; i++ ) {
        m[i] = i;
    }
    j = k = 0;
    for( i = 0; i < 256; i++ ) {
        int a = m[i];
        j = ( j + a + key[k] ) & 0xFF;
        m[i] = m[j];
        m[j] = a;
        if( ++k >= (int)length ) {
            k = 0;
        }
    }
}
void CRYPT_ArcFourCrypt(void* context, unsigned char *data, FX_DWORD length )
{
    struct rc4_state* s = (struct rc4_state*)context;
    int i, x, y, *m;
    x = s->x;
    y = s->y;
    m = s->m;
    for( i = 0; i < (int)length; i++ ) {
        x = ( x + 1 ) & 0xFF;
        int a = m[x];
        y = ( y + a ) & 0xFF;
        m[x] = m[y];
        int b = m[y];
        m[y] = a;
        data[i] ^= m[( a + b ) & 0xFF];
    }
    s->x = x;
    s->y = y;
}
void CRYPT_ArcFourCryptBlock(FX_LPBYTE pData, FX_DWORD size, FX_LPCBYTE key, FX_DWORD keylen)
{
    rc4_state s;
    CRYPT_ArcFourSetup(&s, key, keylen);
    CRYPT_ArcFourCrypt(&s, pData, size);
}
struct md5_context {
    FX_DWORD total[2];
    FX_DWORD state[4];
    FX_BYTE buffer[64];
};
#define FX_CRYPT_GET_FX_DWORD(n,b,i)                                       \
    {                                                               \
        (n) = (FX_DWORD) ((FX_BYTE *) b)[(i)]                           \
              | (((FX_DWORD) ((FX_BYTE *) b)[(i)+1]) <<  8)                 \
              | (((FX_DWORD) ((FX_BYTE *) b)[(i)+2]) << 16)                 \
              | (((FX_DWORD) ((FX_BYTE *) b)[(i)+3]) << 24);                \
    }
#define FX_CRYPT_PUT_FX_DWORD(n,b,i)                                       \
    {                                                               \
        (((FX_BYTE *) b)[(i)]  ) = (FX_BYTE) (((n)      ) & 0xFF);      \
        (((FX_BYTE *) b)[(i)+1]) = (FX_BYTE) (((n) >>  8) & 0xFF);      \
        (((FX_BYTE *) b)[(i)+2]) = (FX_BYTE) (((n) >> 16) & 0xFF);      \
        (((FX_BYTE *) b)[(i)+3]) = (FX_BYTE) (((n) >> 24) & 0xFF);      \
    }
void md5_process( struct md5_context *ctx, const FX_BYTE data[64] )
{
    FX_DWORD A, B, C, D, X[16];
    FX_CRYPT_GET_FX_DWORD( X[0],  data,  0 );
    FX_CRYPT_GET_FX_DWORD( X[1],  data,  4 );
    FX_CRYPT_GET_FX_DWORD( X[2],  data,  8 );
    FX_CRYPT_GET_FX_DWORD( X[3],  data, 12 );
    FX_CRYPT_GET_FX_DWORD( X[4],  data, 16 );
    FX_CRYPT_GET_FX_DWORD( X[5],  data, 20 );
    FX_CRYPT_GET_FX_DWORD( X[6],  data, 24 );
    FX_CRYPT_GET_FX_DWORD( X[7],  data, 28 );
    FX_CRYPT_GET_FX_DWORD( X[8],  data, 32 );
    FX_CRYPT_GET_FX_DWORD( X[9],  data, 36 );
    FX_CRYPT_GET_FX_DWORD( X[10], data, 40 );
    FX_CRYPT_GET_FX_DWORD( X[11], data, 44 );
    FX_CRYPT_GET_FX_DWORD( X[12], data, 48 );
    FX_CRYPT_GET_FX_DWORD( X[13], data, 52 );
    FX_CRYPT_GET_FX_DWORD( X[14], data, 56 );
    FX_CRYPT_GET_FX_DWORD( X[15], data, 60 );
#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))
#define FX_CRYPT_P(a,b,c,d,k,s,t)                                \
    {                                                       \
        a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
    }
    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
#define F(x,y,z) (z ^ (x & (y ^ z)))
    FX_CRYPT_P(A, B, C, D,  0,  7, 0xD76AA478 );
    FX_CRYPT_P(D, A, B, C,  1, 12, 0xE8C7B756 );
    FX_CRYPT_P(C, D, A, B,  2, 17, 0x242070DB );
    FX_CRYPT_P(B, C, D, A,  3, 22, 0xC1BDCEEE );
    FX_CRYPT_P(A, B, C, D,  4,  7, 0xF57C0FAF );
    FX_CRYPT_P(D, A, B, C,  5, 12, 0x4787C62A );
    FX_CRYPT_P(C, D, A, B,  6, 17, 0xA8304613 );
    FX_CRYPT_P(B, C, D, A,  7, 22, 0xFD469501 );
    FX_CRYPT_P(A, B, C, D,  8,  7, 0x698098D8 );
    FX_CRYPT_P(D, A, B, C,  9, 12, 0x8B44F7AF );
    FX_CRYPT_P(C, D, A, B, 10, 17, 0xFFFF5BB1 );
    FX_CRYPT_P(B, C, D, A, 11, 22, 0x895CD7BE );
    FX_CRYPT_P(A, B, C, D, 12,  7, 0x6B901122 );
    FX_CRYPT_P(D, A, B, C, 13, 12, 0xFD987193 );
    FX_CRYPT_P(C, D, A, B, 14, 17, 0xA679438E );
    FX_CRYPT_P(B, C, D, A, 15, 22, 0x49B40821 );
#undef F
#define F(x,y,z) (y ^ (z & (x ^ y)))
    FX_CRYPT_P(A, B, C, D,  1,  5, 0xF61E2562 );
    FX_CRYPT_P(D, A, B, C,  6,  9, 0xC040B340 );
    FX_CRYPT_P(C, D, A, B, 11, 14, 0x265E5A51 );
    FX_CRYPT_P(B, C, D, A,  0, 20, 0xE9B6C7AA );
    FX_CRYPT_P(A, B, C, D,  5,  5, 0xD62F105D );
    FX_CRYPT_P(D, A, B, C, 10,  9, 0x02441453 );
    FX_CRYPT_P(C, D, A, B, 15, 14, 0xD8A1E681 );
    FX_CRYPT_P(B, C, D, A,  4, 20, 0xE7D3FBC8 );
    FX_CRYPT_P(A, B, C, D,  9,  5, 0x21E1CDE6 );
    FX_CRYPT_P(D, A, B, C, 14,  9, 0xC33707D6 );
    FX_CRYPT_P(C, D, A, B,  3, 14, 0xF4D50D87 );
    FX_CRYPT_P(B, C, D, A,  8, 20, 0x455A14ED );
    FX_CRYPT_P(A, B, C, D, 13,  5, 0xA9E3E905 );
    FX_CRYPT_P(D, A, B, C,  2,  9, 0xFCEFA3F8 );
    FX_CRYPT_P(C, D, A, B,  7, 14, 0x676F02D9 );
    FX_CRYPT_P(B, C, D, A, 12, 20, 0x8D2A4C8A );
#undef F
#define F(x,y,z) (x ^ y ^ z)
    FX_CRYPT_P(A, B, C, D,  5,  4, 0xFFFA3942 );
    FX_CRYPT_P(D, A, B, C,  8, 11, 0x8771F681 );
    FX_CRYPT_P(C, D, A, B, 11, 16, 0x6D9D6122 );
    FX_CRYPT_P(B, C, D, A, 14, 23, 0xFDE5380C );
    FX_CRYPT_P(A, B, C, D,  1,  4, 0xA4BEEA44 );
    FX_CRYPT_P(D, A, B, C,  4, 11, 0x4BDECFA9 );
    FX_CRYPT_P(C, D, A, B,  7, 16, 0xF6BB4B60 );
    FX_CRYPT_P(B, C, D, A, 10, 23, 0xBEBFBC70 );
    FX_CRYPT_P(A, B, C, D, 13,  4, 0x289B7EC6 );
    FX_CRYPT_P(D, A, B, C,  0, 11, 0xEAA127FA );
    FX_CRYPT_P(C, D, A, B,  3, 16, 0xD4EF3085 );
    FX_CRYPT_P(B, C, D, A,  6, 23, 0x04881D05 );
    FX_CRYPT_P(A, B, C, D,  9,  4, 0xD9D4D039 );
    FX_CRYPT_P(D, A, B, C, 12, 11, 0xE6DB99E5 );
    FX_CRYPT_P(C, D, A, B, 15, 16, 0x1FA27CF8 );
    FX_CRYPT_P(B, C, D, A,  2, 23, 0xC4AC5665 );
#undef F
#define F(x,y,z) (y ^ (x | ~z))
    FX_CRYPT_P(A, B, C, D,  0,  6, 0xF4292244 );
    FX_CRYPT_P(D, A, B, C,  7, 10, 0x432AFF97 );
    FX_CRYPT_P(C, D, A, B, 14, 15, 0xAB9423A7 );
    FX_CRYPT_P(B, C, D, A,  5, 21, 0xFC93A039 );
    FX_CRYPT_P(A, B, C, D, 12,  6, 0x655B59C3 );
    FX_CRYPT_P(D, A, B, C,  3, 10, 0x8F0CCC92 );
    FX_CRYPT_P(C, D, A, B, 10, 15, 0xFFEFF47D );
    FX_CRYPT_P(B, C, D, A,  1, 21, 0x85845DD1 );
    FX_CRYPT_P(A, B, C, D,  8,  6, 0x6FA87E4F );
    FX_CRYPT_P(D, A, B, C, 15, 10, 0xFE2CE6E0 );
    FX_CRYPT_P(C, D, A, B,  6, 15, 0xA3014314 );
    FX_CRYPT_P(B, C, D, A, 13, 21, 0x4E0811A1 );
    FX_CRYPT_P(A, B, C, D,  4,  6, 0xF7537E82 );
    FX_CRYPT_P(D, A, B, C, 11, 10, 0xBD3AF235 );
    FX_CRYPT_P(C, D, A, B,  2, 15, 0x2AD7D2BB );
    FX_CRYPT_P(B, C, D, A,  9, 21, 0xEB86D391 );
#undef F
    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}
void CRYPT_MD5Start(void* context)
{
    struct md5_context *ctx = (struct md5_context*)context;
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}
void CRYPT_MD5Update(FX_LPVOID pctx, FX_LPCBYTE input, FX_DWORD length )
{
    struct md5_context *ctx = (struct md5_context *)pctx;
    FX_DWORD left, fill;
    if( ! length ) {
        return;
    }
    left = ( ctx->total[0] >> 3 ) & 0x3F;
    fill = 64 - left;
    ctx->total[0] += length <<  3;
    ctx->total[1] += length >> 29;
    ctx->total[0] &= 0xFFFFFFFF;
    ctx->total[1] += ctx->total[0] < length << 3;
    if( left && length >= fill ) {
        FXSYS_memcpy32(ctx->buffer + left,  input, fill );
        md5_process( ctx, ctx->buffer );
        length -= fill;
        input  += fill;
        left = 0;
    }
    while( length >= 64 ) {
        md5_process( ctx, input );
        length -= 64;
        input  += 64;
    }
    if( length ) {
        FXSYS_memcpy32(ctx->buffer + left,  input, length );
    }
}
const FX_BYTE md5_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
void CRYPT_MD5Finish(FX_LPVOID pctx, FX_BYTE digest[16] )
{
    struct md5_context *ctx = (struct md5_context *)pctx;
    FX_DWORD last, padn;
    FX_BYTE msglen[8];
    FX_CRYPT_PUT_FX_DWORD( ctx->total[0], msglen, 0 );
    FX_CRYPT_PUT_FX_DWORD( ctx->total[1], msglen, 4 );
    last = ( ctx->total[0] >> 3 ) & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );
    CRYPT_MD5Update( ctx, md5_padding, padn );
    CRYPT_MD5Update( ctx, msglen, 8 );
    FX_CRYPT_PUT_FX_DWORD( ctx->state[0], digest,  0 );
    FX_CRYPT_PUT_FX_DWORD( ctx->state[1], digest,  4 );
    FX_CRYPT_PUT_FX_DWORD( ctx->state[2], digest,  8 );
    FX_CRYPT_PUT_FX_DWORD( ctx->state[3], digest, 12 );
}
void CRYPT_MD5Generate(FX_LPCBYTE input, FX_DWORD length, FX_BYTE digest[16])
{
    md5_context ctx;
    CRYPT_MD5Start(&ctx);
    CRYPT_MD5Update(&ctx, input, length);
    CRYPT_MD5Finish(&ctx, digest);
}
#ifdef __cplusplus
};
#endif
