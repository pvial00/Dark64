#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "reddye.c"
#include "reddye_kdf.c"

int keylen = 32;
uint64_t r[8] = {0};
uint64_t j = 0;

uint64_t rotate(uint64_t a, int b) {
    return ((a << b) | (a >> (64 - b)));
}

void F(uint64_t j) {
    int i;
    for (i = 0; i < 8; i++) {
        r[i] = (r[i] + r[(i + 1) & 0x07] + j);
        r[i] = r[i] ^ r[(i + 2) & 0x07];
        r[i] = rotate(r[i], 2);
        j = (j + r[i]);
    }
}

void keysetup(unsigned char *key, unsigned char *nonce) {
    uint64_t n[2];
    int i;
    int m = 0;
    int inc = 8;
    for (i = 0; i < (keylen / 8); i++) {
        r[i] = ((uint64_t)(key[m]) << 56) + ((uint64_t)key[m+1] << 48) + ((uint64_t)key[m+2] << 40) + ((uint64_t)key[m+3] << 32) + ((uint64_t)key[m+4] << 24) + ((uint64_t)key[m+5] << 16) + ((uint64_t)key[m+6] << 8) + (uint64_t)key[m+7];
	m += inc;
    }
    
    n[0] = ((uint64_t)nonce[0] << 56) + ((uint64_t)nonce[1] << 48) + ((uint64_t)nonce[2] << 40) + ((uint64_t)nonce[3] << 32) + ((uint64_t)nonce[4] << 24) + ((uint64_t)nonce[5] << 16) + ((uint64_t)nonce[6] << 8) + (uint64_t)nonce[7];
    n[1] = ((uint64_t)nonce[8] << 56) + ((uint64_t)nonce[9] << 48) + ((uint64_t)nonce[10] << 40) + ((uint64_t)nonce[11] << 32) + ((uint64_t)nonce[12] << 24) + ((uint64_t)nonce[13] << 16) + ((uint64_t)nonce[14] << 8) + (uint64_t)nonce[15];

    r[0] = r[0] ^ n[0];
    r[1] = r[1] ^ n[1];

    for (int i = 0; i < 8; i++) {
        j = (j +  r[i]);
    }
    for (int i = 0; i < 64; i++) {
        F(j);
    }
}

void usage() {
    printf("darker <encrypt/decrypt> <input file> <output file> <password>\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    FILE *infile, *outfile, *randfile;
    char *in, *out, *mode;
    unsigned char *data = NULL;
    unsigned char *buf = NULL;
    int x = 0;
    int i = 0;
    int ch;
    int buflen = 131072;
    int bsize;
    uint64_t output[2];
    unsigned char *key[keylen];
    unsigned char *password;
    int nonce_length = 16;
    int iterations = 10;
    int k[8] = {0};
    int o[8] = {0};
    unsigned char *salt = "MyDarkCipher";
    unsigned char nonce[nonce_length];
    unsigned char block[buflen];
    if (argc != 5) {
        usage();
    }
    mode = argv[1];
    in = argv[2];
    out = argv[3];
    password = argv[4];
    infile = fopen(in, "rb");
    fseek(infile, 0, SEEK_END);
    long fsize = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    outfile = fopen(out, "wb");
    int c = 0;
    if (strcmp(mode, "encrypt") == 0) {
        long blocks = fsize / buflen;
        long extra = fsize % buflen;
        if (extra != 0) {
            blocks += 1;
        }
        reddye_random(nonce, nonce_length);
        fwrite(nonce, 1, nonce_length, outfile);
        kdf(password, key, salt, iterations, keylen);
        keysetup(key, nonce);
        for (int d = 0; d < blocks; d++) {
            fread(block, buflen, 1, infile);
            bsize = sizeof(block);
	    c = 0;
            for (int b = 0; b < (bsize / 8); b++) {
		F(j);
		output[0] = ((((r[1] + r[7]) ^ r[3]) ^ r[5]));
		k[0] = (output[0] & 0x00000000000000FF);
		k[1] = (output[0] & 0x000000000000FF00) >> 8;
		k[2] = (output[0] & 0x0000000000FF0000) >> 16;
		k[3] = (output[0] & 0x00000000FF000000) >> 24;
		k[4] = (output[0] & 0x000000FF00000000) >> 32;
		k[5] = (output[0] & 0x0000FF0000000000) >> 40;
		k[6] = (output[0] & 0x00FF000000000000) >> 48;
		k[7] = (output[0] & 0xFF00000000000000) >> 56;
		for (c = (b * 8); c < ((b *8) + 8); c++) {
                    block[c] = block[c] ^ k[c & 0x07];
		}
            }
            if (d == (blocks - 1) && extra != 0) {
                bsize = extra;
            }
            fwrite(block, 1, bsize, outfile);
        }
    }
    else if (strcmp(mode, "decrypt") == 0) {
        long blocks = (fsize - nonce_length) / buflen;
        long extra = (fsize - nonce_length) % buflen;
        if (extra != 0) {
            blocks += 1;
        }
        fread(nonce, 1, nonce_length, infile);
        kdf(password, key, salt, iterations, keylen);
        keysetup(key, nonce);
        for (int d = 0; d < blocks; d++) {
            fread(block, buflen, 1, infile);
            bsize = sizeof(block);
            for (int b = 0; b < (bsize / 8); b++) {
		F(j);
		output[0] = ((((r[1] + r[7]) ^ r[3]) ^ r[5]));
		k[0] = (output[0] & 0x00000000000000FF);
		k[1] = (output[0] & 0x000000000000FF00) >> 8;
		k[2] = (output[0] & 0x0000000000FF0000) >> 16;
		k[3] = (output[0] & 0x00000000FF000000) >> 24;
		k[4] = (output[0] & 0x000000FF00000000) >> 32;
		k[5] = (output[0] & 0x0000FF0000000000) >> 40;
		k[6] = (output[0] & 0x00FF000000000000) >> 48;
		k[7] = (output[0] & 0xFF00000000000000) >> 56;
		for (c = (b * 8); c < ((b *8) + 8); c++) {
                    block[c] = block[c] ^ k[c & 0x07];
		}
            }
            if ((d == (blocks - 1)) && extra != 0) {
                bsize = extra;
            }
            fwrite(block, 1, bsize, outfile);
        }
    }
    fclose(infile);
    fclose(outfile);
    return 0;
}
