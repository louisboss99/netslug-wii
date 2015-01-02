/* main.c
 *   by Alex Chadwick
 * 
 * Copyright (C) 2013-2015, Alex Chadwick
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

#ifndef SETTINGS_H_
#define SETTINGS_H_

typedef enum settings_variableType_t {
  settings_variableType_int,
  settings_variableType_bool, /* implemented with int */
  settings_variableType_charPtr,
  settings_variableType_ip, /* IPv4 int */
} settings_variableType_t;

typedef struct settings_category_t {
  const char *name; /*!< up to 32 characters. */
  struct settings_value_t *settings;
  unsigned int count;
} settings_category_t;

typedef struct settings_value_t {
  int hidden;
  const char *name; /*!< up to 32 characters. */
  void *address;
  settings_variableType_t type;
  int min, max;
  const char *comment;
  union settings_defaultValue_t {
    int _int;
    void *_ptr;
  } defaultValue;
} settings_value_t;

extern const settings_category_t settings_categories[];
extern const unsigned int settings_categoriesCount;

/** To be called once before any other method here. */
void settings_init(void);
void settings_load(void);
void settings_save(void);

#endif /* SETTINGS_H_ */
