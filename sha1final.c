#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ROTL32(x,n) (((x) << (n)) | ((x) >> (32-(n))))

static uint32_t load_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |
           ((uint32_t)p[3]);
}

static void sha1_message_schedule(
    const uint8_t block[64],
    uint32_t W[80]
) {
    for (int t = 0; t < 16; t++) {
        W[t] = load_be32(&block[t * 4]);
    }

    for (int t = 16; t < 80; t++) {
        W[t] = ROTL32(
            W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16],
            1
        );
    }
}

static uint32_t sha1_f(int t, uint32_t b, uint32_t c, uint32_t d) {
    if (t < 20) {
        return (b & c) | (~b & d);
    }
    else if (t < 40) {
        return b ^ c ^ d;
    }
    else if (t < 60) {
        return (b & c) | (b & d) | (c & d);
    }
    else {
        return b ^ c ^ d;
    }
}

static void sha1_compress(uint32_t state[5], const uint8_t block[64]) {

    uint32_t W[80];

    sha1_message_schedule(block, W);

    uint32_t A = state[0];
    uint32_t B = state[1];
    uint32_t C = state[2];
    uint32_t D = state[3];
    uint32_t E = state[4];

    for (int t = 0; t < 80; t++) {

        uint32_t K;

        if (t < 20) {
            K = 0x5A827999;
        }
        else if (t < 40) {
            K = 0x6ED9EBA1;
        }
        else if (t < 60) {
            K = 0x8F1BBCDC;
        }
        else {
            K = 0xCA62C1D6;
        }

        uint32_t temp = ROTL32(A, 5)
                      + sha1_f(t, B, C, D)
                      + E + K + W[t];

        E = D;
        D = C;
        C = ROTL32(B, 30);
        B = A;
        A = temp;
    }

    state[0] += A;
    state[1] += B;
    state[2] += C;
    state[3] += D;
    state[4] += E;
}

int sha1(const uint8_t *data, size_t length, uint8_t hash[20]) {

    if (hash == NULL || (data == NULL && length > 0)) {
        return 0;
    }

    if (length > UINT64_MAX / 8 || length > SIZE_MAX - 72) {
        return 0;
    }

    uint32_t state[5] = {
        0x67452301,
        0xEFCDAB89,
        0x98BADCFE,
        0x10325476,
        0xC3D2E1F0
    };

    size_t paddedLength = ((length + 72) / 64) * 64;

    uint8_t *padded = calloc(paddedLength, 1);

    if (padded == NULL) {
        return 0;
    }

    if (length > 0) {
        memcpy(padded, data, length);
    }

    padded[length] = 0x80;

   
    uint64_t bitLength = (uint64_t)length * 8;

  
    for (int i = 0; i < 8; i++) {
        padded[paddedLength - 1 - i] =
            (uint8_t)(bitLength >> (8 * i));
    }

    for (size_t i = 0; i < paddedLength; i += 64) {
        sha1_compress(state, &padded[i]);
    }

    for (int i = 0; i < 5; i++) {
        hash[i * 4]     = (uint8_t)(state[i] >> 24);
        hash[i * 4 + 1] = (uint8_t)(state[i] >> 16);
        hash[i * 4 + 2] = (uint8_t)(state[i] >> 8);
        hash[i * 4 + 3] = (uint8_t)state[i];
    }

    free(padded);

    return 1;
}

int main(void) {
    char input[4096];
    uint8_t hash[20];

    printf("Enter input: ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
        size_t len = strcspn(input, "\r\n");
        input[len] = '\0';

        if (sha1((const uint8_t *)input, len, hash)) {
            printf("SHA-1 digest: ");
            for (int i = 0; i < 20; i++) {
                printf("%02x", hash[i]);
            }
            printf("\n");
        } else {
            printf("Hashing failed.\n");
        }
    }

    return 0;
}
