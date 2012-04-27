
/*
 * GDB Stub xfer-spaces support code.
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

typedef struct XferObject {

  /* This function shall return the name of the object. The name is 
     considered to be a hierarchical separated by dots.  */
  const char* (*get_name)(void* opaque);

  /* This function shall return the data size (bytes) of 
     this object.  */
  unsigned (*get_size)(void* opaque);

  /* Read the data contained in the object. Return boolean success status.  */
  int (*read)(void* opaque, unsigned char* buf);

  /* Write the data into the object. Return boolean success status. */
  int (*write)(void* opaque, const unsigned char* buf);

} XferObject;

/* Declare an object in a xfer:space. */
extern void xfer_spaces_declare_object(const char* space_name,
                                       XferObject* object, void* opaque);

/* Dump xfer:space XML description. */
extern const char* xfer_spaces_get_xml();

/* Read/Write operation on object(s) addressed by thier space and offset */
extern int xfer_spaces_read_write(int is_read, const char* space_name,
                                  unsigned offset, unsigned char* data,
                                  unsigned length);

#ifdef __cplusplus
}
#endif
