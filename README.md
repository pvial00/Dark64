# Dark64

Dark64 is a 256 bit stream cipher aimed at providing no vector of attack other than brute force.  Dark64 is part of the Dark family of ciphers.  Dark maintains a 512 bit state and outputs 64 bits from a complex output function that operates on half of the state (256 bits),  This leaves no room for clues regarding the state.  Dark is fast and formidable.  Dark takes a 256 bit key and 128 bit nonce.

In order to break one 64 bit round of Dark64, one must discover the 256 bit half state.  In order to calculate the round after it or before it, all 512 bits of the state is needed.  Best attack vector is brute force.

# Algorithm description

Dark initializes a 512 bit array (8 x 64 bit words) with the key.  One quarterof the array is XORed with the nonce and first round begins.  Each round, each array element is added to the element to the right of it, XORed with the value prior to the addition and then rotated left 2 bits.  The odd half of the 8, 64 bit words are then added and XORed together to create 64 output bits which is XORed with the 8 input bytes.

The complexity to solve for the state at any given block of 64 bits in the ciphertext is the same complexity to brute force the key.
