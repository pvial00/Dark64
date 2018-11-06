# Dark64

Dark64 is a 256 bit stream cipher aimed at providing no vector of attack other than brute force.  Dark64 is part of the Dark family of ciphers.  Dark maintains a 512 bit state and outputs 64 bits from a complex output function that operates on half of the state (256 bits),  This leaves no room for clues regarding the state.  Dark is fast and formidable.  Dark takes a 256 bit key and 128 bit nonce.

In order to break one 64 bit round of Dark64, one must discover the 256 bit half state.  In order to calculate the round after it or before it, all 512 bits of the state is needed.  Best attack vector, at the moment, is brute force.

# Algorithm description

Dark initializes a 512 bit array (8 x 64 bit words) with the unpacked 256 bit key.  One quarter of the array is XORed with the nonce and then the round function is applied 64 times before beginning encryption or decryption.  The resulting 512 bit array is called the register or state but I prefer register.  The register's function is to output pseudo random bits to be XORed with the plaintext.

For each round of encryption, the round function is applied to the entire register array in the following manner.  The first element is added to the element to the right.  The result is XORed with the value prior to the addition and then rotated left 2 bits.  Next, the register outputs a 64 bit word by adding the first and 7th words, the result is XORed with the 3rd word and that result is XORed with the fifth word.  Lastly, 8 bytes are unpacked from the 64 bit word and XORed with the first 8 plaintext bytes.  This process repeats until there are no more plaintext bytes to process.  Remaining bytes of the keystream are discarded.

Round function demonstrated in C code:

void F(uint64_t j, uint64_t ct) {

    int i;

    uint64_t x;

    for (i = 0; i < 8; i++) {

        x = r[i];

        r[i] = (r[i] + r[(i + 1) & 0x07] + j);

        r[i] = r[i] ^ x;

        r[i] = rotate(r[i], 2);

        j = (j + r[i] + ct);

        ct = (ct + 1);

    }

}

The complexity to solve for the state at any given block of 64 bits in the ciphertext is the same complexity to brute force the key (2^256 complexity).  The complexity to calculate the state or to compute the next state at any given point in time is 2^512.
