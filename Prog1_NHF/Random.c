#include "Random.h"
#include <time.h>
#include <math.h>

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
    return (Rand_uint64(rand) >> 40) * (1.f / 16777215.f);
}

float Rand_float_2(MT64_Gen_t* rand)
{
    return (Rand_uint64(rand) >> 40) * (1.f / 16777216.f);
}

float Rand_float_3(MT64_Gen_t* rand)
{
    return ((Rand_uint64(rand) >> 41) + 0.5f) * (1.f / 8388608.f);
}

MT64_Gen_t global_gen;

void Rand_Init()
{
    init_genrand64(&global_gen, 4503599627370496ull);
}

float RandF_1() { return Rand_float_1(&global_gen); }
float RandF_2() { return Rand_float_2(&global_gen); }
float RandF_3() { return Rand_float_3(&global_gen); }
float RandF_1_Range(float min, float max) { return min + (max - min) * RandF_1(); }
float RandF_2_Range(float min, float max) { return min + (max - min) * RandF_2(); }
float RandF_3_Range(float min, float max) { return min + (max - min) * RandF_3(); }

double RandD_1() { return Rand_double_1(&global_gen); }
double RandD_2() { return Rand_double_2(&global_gen); }
double RandD_3() { return Rand_double_3(&global_gen); }
double RandD_1_Range(double min, double max) { return min + (max - min) * RandD_1(); }
double RandD_2_Range(double min, double max) { return min + (max - min) * RandD_2(); }
double RandD_3_Range(double min, double max) { return min + (max - min) * RandD_3(); }

bool RandB() { return RandF_1() > 0.5f; }
bool RandB_Threshold(float threshold) { return RandF_1() > threshold; }
bool RandB_Chance(float chance) { return RandF_1() < chance; }

int32_t RandI32() { return Rand_int32(&global_gen); }
int32_t RandI32_Range(int32_t min, int32_t max) { return min + (int32_t)roundf((float)(max - min) * RandF_1()); }

uint32_t RandUI32() { return Rand_uint32(&global_gen); }
uint32_t RandUI32_Range(uint32_t min, uint32_t max) { return min + (uint32_t)roundf((float)(max - min) * RandF_1()); }

int64_t RandI64() { return Rand_int64(&global_gen); }
int64_t RandI64_Range(int64_t min, int64_t max) { return min + (int64_t)round((double)(max - min) * RandD_1()); }

uint64_t RandUI64() { return Rand_uint64(&global_gen); }
uint64_t RandUI64_Range(uint64_t min, uint64_t max) { return min + (uint64_t)round((double)(max - min) * RandD_1()); }
