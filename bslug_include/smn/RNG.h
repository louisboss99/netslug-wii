/* RNG.h
 *   by Alex Chadwick
 * 
 * Copyright (C) 2014, Alex Chadwick
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* definitions of symbols inferred to exist in the RNG.h header file for
 * which the brainslug symbol information is available which are specific to
 * the game `New Super Mario Bros. Wii'.  */

#ifndef _SMN_RNG_H_
#define _SMN_RNG_H_

typedef long rng_t;
/*
// Default non-rentrant RNG
void RNG_DefaultSeed(long seed);
// Returns a random float between 0 and 1
float RNG_DefaultRandFloat(void);
unsigned short RNG_DefaultRand(void);
float RNG_DefaultRandFloatRange(float max);
*/

// Rentrant RNG
unsigned int RNG_Rand(rng_t *rng, unsigned int max);
// Returns a random float between 0 and 1
float RNG_RandFloat(rng_t *rng);
// Returns a random float between -0.5 and 0.5
float RNG_RandFloatZeroed(rng_t *rng);

#endif /* _SMN_RNG_H_ */
