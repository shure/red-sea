
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

#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TextBuffer {
  char *begin, *end, *ptr;
} TextBuffer;

/* Init a text buffer object with an initial capacity. */
void text_buffer_init(TextBuffer* obj, unsigned initial_capacity);

/* Free the allocated memory. */
void text_buffer_free(TextBuffer* obj);

/* Clear the text in the buffer. */
void text_buffer_clear(TextBuffer* obj);

/* Append to the buffer by a printf formatted string. */
void text_buffer_printf(TextBuffer* obj, const char* format, ...);

/* Append to the buffer by a specified string. */
void text_buffer_append(TextBuffer* obj, const char* string);

/* Return size of text in the text buffer. */
unsigned long text_buffer_size(TextBuffer* obj);
