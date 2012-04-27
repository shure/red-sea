
/*
 * Text buffer utility package.
 *
 * Copyright (c) 2012 Alex Rozenman <alex_rozenman@mentor.com>
 * Copyright (c) 2012 Mentor Graphics Corp.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "text-buffer.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define MIN_CAPACITY (4 * 1024)

void text_buffer_init(TextBuffer* obj, unsigned initial_capacity)
{
  if (initial_capacity < MIN_CAPACITY)
    initial_capacity = MIN_CAPACITY;
  obj->begin = (char*)malloc(initial_capacity);
  obj->end = obj->begin + initial_capacity;
  obj->ptr = obj->begin;
}

void text_buffer_free(TextBuffer* obj)
{
  if (obj->begin)
    free(obj->begin);
  obj->begin = obj->end = obj->ptr = 0;
}

void text_buffer_clear(TextBuffer* obj)
{
  obj->ptr = obj->begin;
}

static void make_room(TextBuffer* obj, unsigned needed_room)
{
  const unsigned size = obj->ptr - obj->begin;
  unsigned capacity = obj->end - obj->begin;
  while ((capacity - size) < needed_room) {
    capacity *= 2;
  }

  /* Reallocate. */
  obj->begin = (char*)realloc(obj->begin, capacity);
  obj->end = obj->begin + capacity;
  obj->ptr = obj->begin + size;
}

void text_buffer_printf(TextBuffer* obj, const char* format, ...)
{
  va_list args;

  make_room(obj, 1024);
  va_start(args, format);
  obj->ptr += vsprintf(obj->ptr, format, args);
  va_end (args);
}

void text_buffer_append(TextBuffer* obj, const char* string)
{
  unsigned string_length = strlen(string);
  make_room(obj, string_length + 1);
  strncpy(obj->ptr, string, string_length);
  obj->ptr += string_length;
}
