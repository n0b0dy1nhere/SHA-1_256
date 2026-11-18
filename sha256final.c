#ifndef SHA256_H
#define SHA256_H
#include <stdint.h>
#include <stddef.h>

int sha256(const uint8_t *data, size_t length,
           uint8_t hash[32]);

#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define SHR32(x, n) ((x) >> (n))

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))

#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define SIGMA0(x) (ROTR32(x, 2) ^ ROTR32(x, 13) ^ ROTR32(x, 22))
#define SIGMA1(x) (ROTR32(x, 6) ^ ROTR32(x, 11) ^ ROTR32(x, 25))

#define sigma0(x) (ROTR32(x, 7) ^ ROTR32(x, 18) ^ SHR32(x, 3))
#define sigma1(x) (ROTR32(x, 17) ^ ROTR32(x, 19) ^ SHR32(x, 10))

static uint32_t load_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |
           ((uint32_t)p[3]);
}

static void sha256_message_schedule(
    const uint8_t block[64],
    uint32_t W[64]
) {
    for (int i = 0; i < 16; i++) {
        W[i] = load_be32(&block[i * 4]);
    }

    for (int i = 16; i < 64; i++) {
        W[i] = sigma1(W[i - 2])
             + W[i - 7]
             + sigma0(W[i - 15])
             + W[i - 16];
    }
}


static const uint32_t SHA256_INITIAL_HASH[8] = {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
};
static const uint32_t SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_compress(
    uint32_t H[8],
    const uint8_t block[64]
) {
    uint32_t W[64];

    sha256_message_schedule(block, W);

    uint32_t a = H[0];
    uint32_t b = H[1];
    uint32_t c = H[2];
    uint32_t d = H[3];
    uint32_t e = H[4];
    uint32_t f = H[5];
    uint32_t g = H[6];
    uint32_t h = H[7];

    for (int i = 0; i < 64; i++) {

        uint32_t T1 = h + SIGMA1(e)
                        + CH(e, f, g)
                        + SHA256_K[i]
                        + W[i];

        uint32_t T2 = SIGMA0(a)
                        + MAJ(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + T1;

        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;
}
int sha256(const uint8_t *message, size_t length,
           uint8_t hash[32]) {

    if (hash == NULL || (message == NULL && length != 0)) {
        return 0;
    }

    if (length > UINT64_MAX / 8) {
        return 0;
    }

    if (length > SIZE_MAX - 72) {
        return 0;
    }

    size_t paddedLength = length + 1 + 8;

    while (paddedLength % 64 != 0) {
        paddedLength++;
    }

    uint8_t *data = (uint8_t *)malloc(paddedLength);

    if (data == NULL) {
        return 0;
    }

    if (length > 0) {
        memcpy(data, message, length);
    }

    data[length] = 0x80;

    memset(data + length + 1, 0,
           paddedLength - length - 1);

    uint64_t bitLength = (uint64_t)length * 8;

    for (int i = 0; i < 8; i++) {
        data[paddedLength - 1 - i] =
            (uint8_t)(bitLength >> (i * 8));
    }

    uint32_t H[8];

    for (int i = 0; i < 8; i++) {
    H[i] = SHA256_INITIAL_HASH[i];
}

    for (size_t i = 0; i < paddedLength; i += 64) {
        sha256_compress(H, data + i);
    }

    for (int i = 0; i < 8; i++) {
        hash[i * 4]     = (uint8_t)(H[i] >> 24);
        hash[i * 4 + 1] = (uint8_t)(H[i] >> 16);
        hash[i * 4 + 2] = (uint8_t)(H[i] >> 8);
        hash[i * 4 + 3] = (uint8_t)(H[i]);
    }

    free(data);

    return 1;
}

int main(void) {
    char input[4096];
    uint8_t hash[32];

    printf("Enter input: ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        size_t len = strcspn(input, "\r\n");
        input[len] = '\0';

        if (sha256((const uint8_t *)input, len, hash)) {
            printf("SHA-256 digest: ");
            for (int i = 0; i < 32; i++) {
                printf("%02x", hash[i]);
            }
            printf("\n");
        } else {
            printf("Hashing failed.\n");
        }
    }

    return 0;
}
