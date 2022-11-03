#pragma once
#include "Core.h"
#include "Log.h"

#include <stdint.h>
#include <stdbool.h>

/*
* Mersenne twister because srand can only be seeded once.
* Modified refecence implementation to support more numbers and have the generator in
* a separate object as opposed to global state. Licence of the original implementation is bellow.
*/

/*
   A C-program for MT19937-64 (2004/9/29 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/

#include <stdio.h>

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */

/* Random number generator state. */
typedef struct MT64_Gen_t 
{
    /* The array for the state vector */
    uint64_t mt[NN];
    /* mti==NN+1 means mt[NN] is not initialized */
    int mti;
} MT64_Gen_t;

/* initializes mt[NN] with a seed */
void init_genrand64(MT64_Gen_t* rand, uint64_t seed);

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array64(MT64_Gen_t* rand, uint64_t init_key[],
    uint64_t key_length);

/* generates a random number on [0, 2^64-1]-interval */
uint64_t Rand_uint64(MT64_Gen_t* rand);

/* generates a random number on [0, 2^63-1]-interval */
int64_t Rand_int64(MT64_Gen_t* rand);

/* generates a random number on [0, 2^32-1]-interval */
uint32_t Rand_uint32(MT64_Gen_t* rand);

/* generates a random number on [0, 2^31-1]-interval */
int32_t Rand_int32(MT64_Gen_t* rand);

/* generates a random number on [0,1]-real-interval */
double Rand_double_1(MT64_Gen_t* rand);

/* generates a random number on [0,1)-real-interval */
double Rand_double_2(MT64_Gen_t* rand);

/* generates a random number on (0,1)-real-interval */
double Rand_double_3(MT64_Gen_t* rand);

/* generates a random number on [0,1]-real-interval */
float Rand_float_1(MT64_Gen_t* rand);

/* generates a random number on [0,1)-real-interval */
float Rand_float_2(MT64_Gen_t* rand);

/* generates a random number on (0,1)-real-interval */
float Rand_float_3(MT64_Gen_t* rand);


/*
* User-friendly API now that we don't need the ability to seed the RNG.
*/
void Rand_Init();

float RandF_1();
float RandF_2();
float RandF_3();
float RandF_1_Range(float min, float max);;
float RandF_2_Range(float min, float max);;
float RandF_3_Range(float min, float max);;

double RandD_1();
double RandD_2();
double RandD_3();
double RandD_1_Range(double min, double max);
double RandD_2_Range(double min, double max);
double RandD_3_Range(double min, double max);

bool RandB();;
bool RandB_Threshold(float threshold);

int32_t RandI32();
int32_t RandI32_Range(int32_t min, int32_t max);

uint32_t RandUI32();
uint32_t RandUI32_Range(uint32_t min, uint32_t max);

int64_t RandI64();
int64_t RandI64_Range(int64_t min, int64_t max);

uint64_t RandUI64();
uint64_t RandUI64_Range(uint64_t min, uint64_t max);
