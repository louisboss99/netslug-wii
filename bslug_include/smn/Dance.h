/* Dance.h
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

/* definitions of symbols inferred to exist in the Dance.h header file for
 * which the brainslug symbol information is available which are specific to
 * the game `New Super Mario Bros. Wii'.  */

#ifndef _SMN_DANCE_H_
#define _SMN_DANCE_H_

#include <stdbool.h>

//void SMNDance_Init(void);
//void SMNDance_Update(void);
unsigned char SMNDance_GetUnkown_a6c9(void);
bool SMNDance_GetUnkown_a6c9Mask(unsigned char mask);
unsigned char SMNDance_GetMask(void);
bool SMNDance_Enabled(unsigned char mask);
unsigned char SMNDance_GetUnkown_a6cb(void);
unsigned short SMNDance_GetUnkown_82ae(void);
void SMNDance_SetUnkown_82b0(int value);
int SMNDance_GetUnkown_82b0(void);
void SMNDance_SetUnkown_a6cc(int value);
int SMNDance_GetUnkown_a6cc(void);


#endif /* _SMN_DANCE_H_ */
