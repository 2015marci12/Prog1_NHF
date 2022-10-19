#include "Random.h"

/* initializes mt[NN] with a seed */
void init_genrand64(MT64_Gen_t* rand, uint64_t seed)
{
    rand->mt[0] = seed;
    for (rand->mti = 1; rand->mti < NN; rand->mti++)
        rand->mt[rand->mti] = (6364136223846793005ULL *
            (rand->mt[rand->mti - 1] ^
                (rand->mt[rand->mti - 1] >> 62))
            + rand->mti);
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array64(MT64_Gen_t* rand, uint64_t init_key[], uint64_t key_length)
{
    uint64_t i, j, k;
    init_genrand64(rand, 19650218ULL);
    i = 1; j = 0;
    k = (NN > key_length ? NN : key_length);
    for (; k; k--) {
        rand->mt[i] = (rand->mt[i] ^ ((rand->mt[i - 1] ^ (rand->mt[i - 1] >> 62)) * 3935559000370003845ULL))
            + init_key[j] + j; /* non linear */
        i++; j++;
        if (i >= NN) { rand->mt[0] = rand->mt[NN - 1]; i = 1; }
        if (j >= key_length) j = 0;
    }
    for (k = NN - 1; k; k--) {
        rand->mt[i] = (rand->mt[i] ^ ((rand->mt[i - 1] ^ (rand->mt[i - 1] >> 62)) * 2862933555777941757ULL))
            - i; /* non linear */
        i++;
        if (i >= NN) { rand->mt[0] = rand->mt[NN - 1]; i = 1; }
    }

    rand->mt[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */
}

uint64_t Rand_uint64(MT64_Gen_t* rand)
{
    int i;
    uint64_t x;
    static uint64_t mag01[2] = { 0ULL, MATRIX_A };

    if (rand->mti >= NN) { /* generate NN words at one time */

        /* if init_genrand64() has not been called, */
        /* a default initial seed is used     */
        if (rand->mti == NN + 1)
            init_genrand64(rand, 5489ULL);

        for (i = 0; i < NN - MM; i++) {
            x = (rand->mt[i] & UM) | (rand->mt[i + 1] & LM);
            rand->mt[i] = rand->mt[i + MM] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];
        }
        for (; i < NN - 1; i++) {
            x = (rand->mt[i] & UM) | (rand->mt[i + 1] & LM);
            rand->mt[i] = rand->mt[i + (MM - NN)] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];
        }
        x = (rand->mt[NN - 1] & UM) | (rand->mt[0] & LM);
        rand->mt[NN - 1] = rand->mt[MM - 1] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];

        rand->mti = 0;
    }

    x = rand->mt[rand->mti++];

    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);

    return x;
}

int64_t Rand_int64(MT64_Gen_t* rand)
{
    return (int64_t)(Rand_uint64(rand) >> 1);
}

uint32_t Rand_uint32(MT64_Gen_t* rand)
{
    return (uint32_t)(Rand_uint64(rand) >> 32);
}

int32_t Rand_int32(MT64_Gen_t* rand)
{
    return (int32_t)(Rand_uint64(rand) >> 33);
}

double Rand_double_1(MT64_Gen_t* rand)
{
    return (Rand_uint64(rand) >> 11) * (1.0 / 9007199254740991.0);
}

double Rand_double_2(MT64_Gen_t* rand)
{
    return (Rand_uint64(rand) >> 11) * (1.0 / 9007199254740992.0);
}

double Rand_double_3(MT64_Gen_t* rand)
{
    return ((Rand_uint64(rand) >> 12) + 0.5) * (1.0 / 4503599627370496.0);
}

float Rand_float_1(MT64_Gen_t* rand)
{
    return (Rand_uint64(rand) >> 35) * (1.f / 9007199254740991.f);
}

float Rand_float_2(MT64_Gen_t* rand)
{
    return (Rand_uint64(rand) >> 35) * (1.f / 9007199254740992.f);
}

float Rand_float_3(MT64_Gen_t* rand)
{
    return ((Rand_uint64(rand) >> 35) + 0.5f) * (1.f / 4503599627370496.f);
}
