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

#include "xfer-spaces.h"
#include "text-buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define NAME_SEPARATOR "."
#define INITIAL_OFFSET 0x10

typedef struct XferRecord
{
  struct XferRecord* next;
  XferObject* object;
  void* opaque;
  unsigned offset;
} XferRecord;

typedef struct XferTreeNode
{
  struct XferTreeNode* sibling;
  struct XferTreeNode* child;
  XferRecord* record;
  char* name;
} XferTreeNode;

typedef struct XferSpace
{
  struct XferSpace* next;
  const char* name;
  XferRecord *list;
  XferTreeNode* tree;
  XferRecord **map;
  unsigned map_size;
} XferSpace;

XferSpace* xfer_spaces;

static XferSpace* get_space(const char* name)
{
  XferSpace* ptr;

  for (ptr = xfer_spaces; ptr; ptr = xfer_spaces->next) {
    if (!strcmp(name, ptr->name))
      return ptr;
  }
  
  ptr = (XferSpace*)calloc(1, sizeof(XferSpace));
  ptr->next = xfer_spaces;
  ptr->name = name;
  xfer_spaces = ptr;
  return ptr;
}

static XferRecord* alloc_record(XferObject* debuggable, void* opaque)
{
  XferRecord* record =
    (XferRecord*)calloc(1, sizeof(XferRecord));
  record->object = debuggable;
  record->opaque = opaque;
  record->offset = 0;
  return record;
}

static XferTreeNode* alloc_node()
{
  return (XferTreeNode*)calloc(1, sizeof(XferTreeNode));
}

static void dealloc_node(XferTreeNode* node)
{
  if (node->name)
    free(node->name);
  if (node->sibling)
    dealloc_node(node->sibling);
  if (node->child)
    dealloc_node(node->child);
  free(node);
}

static void space_discard_map(XferSpace* space)
{
  if (space->map) {
    free(space->map);
    space->map = 0;
  }
  space->map_size = 0;
}

static void space_discard_tree(XferSpace* space)
{
  if (space->tree) {
    dealloc_node(space->tree);
    space->tree = 0;
  }
}

void xfer_spaces_declare_object(const char* space_name, XferObject* debuggable, void* opaque)
{
  XferRecord* ptr;
  XferSpace* space = get_space(space_name);
  space_discard_map(space);
  space_discard_tree(space);

  /* Add to the initial list. */
  ptr = alloc_record(debuggable, opaque);
  ptr->next = space->list;
  space->list = ptr;
}

static XferTreeNode* find_sibling(XferTreeNode* node, const char* name)
{
  XferTreeNode *ptr;
  for (ptr = node; ptr; ptr = ptr->sibling) {
    if (ptr->name && !strcmp(name, ptr->name)) {
      return ptr;
    }
  }
  return 0;
}

static void merge_record(XferTreeNode* ptr, XferRecord* record)
{
  char *saveptr = 0, *token;
  char* path = strdup(record->object->get_name(record->opaque));

  token = strtok_r(path, NAME_SEPARATOR, &saveptr);
  while (token) {
    XferTreeNode *sibling = find_sibling(ptr->child, token);
    if (!sibling) {
      sibling = alloc_node();
      sibling->name = strdup(token);
      sibling->sibling = ptr->child;
      ptr->child = sibling;
      ptr = sibling;
    }
    ptr = sibling;
    token = strtok_r(0, NAME_SEPARATOR, &saveptr);
  }

  free(path);

  ptr->record = record;
}

static void space_create_map(XferSpace* space)
{
  XferRecord *ptr;
  unsigned offset = INITIAL_OFFSET;

  /* Set offsets and merge into the tree. */
  for (ptr = space->list; ptr; ptr = ptr->next) {
    ptr->offset = offset;
    offset += ptr->object->get_size(ptr->opaque);
  }

  /* Create the map. */
  space->map_size = offset;
  space->map = (XferRecord**)calloc(space->map_size, sizeof(*space->map));
  for (ptr = space->list; ptr; ptr = ptr->next) {
    space->map[ptr->offset] = ptr;
  }
}

static void space_create_tree(XferSpace* space)
{
  XferRecord *ptr;

  /* Create the tree root. */
  space->tree = alloc_node();

  /* Merge into the tree. */
  for (ptr = space->list; ptr; ptr = ptr->next) {
    merge_record(space->tree, ptr);
  }
}

static TextBuffer xml;

static void xml_print_indent(unsigned indent)
{
  unsigned i;
  for (i = 0; i < indent; ++i) {
    text_buffer_append(&xml, " ");
  }
}

static void xml_dump_node(XferTreeNode* node, unsigned indent)
{
  XferTreeNode *ptr;
  for (ptr = node; ptr; ptr = ptr->sibling) {
    xml_print_indent(indent);
    if (ptr->record) {
      unsigned data_size = ptr->record->object->get_size(ptr->record->opaque);
      text_buffer_printf(&xml, "<reg name=\"%s\" offset=\"%d\" bitsize=\"%d\"/>\n",
                         ptr->name, ptr->record->offset, data_size * 8);
    } else {
      if (ptr->name) {
        text_buffer_printf(&xml, "<group name=\"%s\">\n", ptr->name);
        xml_dump_node(ptr->child, indent + 2);
        xml_print_indent(indent);
        text_buffer_printf(&xml, "</group>\n");
      } else {
        xml_dump_node(ptr->child, indent);
      }
    }
  }
}

const char* xfer_spaces_get_xml()
{
  XferSpace* ptr;
  int need_dump = 0;;

  if (!xml.begin) {
    text_buffer_init(&xml, 4*1024);
  }

  for (ptr = xfer_spaces; ptr; ptr = ptr->next) {
    if (!ptr->map) {
      space_create_map(ptr);
      need_dump = 1;
    }
    if (!ptr->tree) {
      space_create_tree(ptr);
      need_dump = 1;
    }
  }

  if (need_dump) {
    text_buffer_clear(&xml);
    text_buffer_append(&xml, "<?xml version=\"1.0\"?>\n");
    text_buffer_append(&xml, "<!DOCTYPE feature SYSTEM \"gdb-target.dtd\">\n");
    text_buffer_append(&xml, "<feature name=\"org.gnu.gdb.xfer-spaces\">\n");
    for (ptr = xfer_spaces; ptr; ptr = ptr->next) {
      text_buffer_printf(&xml, "<space annex=\"%s\" name=\"%s\">\n", ptr->name, ptr->name);
      xml_dump_node(ptr->tree, 0);
      text_buffer_printf(&xml, "</space>\n");
    }
    text_buffer_append(&xml, "</feature>\n");
  }
  
  return xml.begin;
}

int xfer_spaces_read_write(int is_read, const char* space_name, unsigned offset,
                           unsigned char* data, unsigned length)
{
  XferRecord* record;
  XferSpace* space = get_space(space_name);

  if (!space->map)
    space_create_map(space);

  if (offset + length >= space->map_size)
    return 0;

  record = space->map[offset];
  if (!record || !record->object->read)
    return 0;

  if (record->object->get_size(record->opaque) != length)
    return 0;
  
  if (is_read)
    return record->object->read(record->opaque, data);
  else 
    return record->object->write(record->opaque, data);
}

