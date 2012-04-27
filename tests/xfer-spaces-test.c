
#include "../utils/xfer-spaces.h"
#include <stdlib.h>

/*
 * For test use:
 * gcc xfer-spaces-test.c ../utils/xfer-spaces.c ../utils/text-buffer.c -I ../utils -o /tmp/test -O3 -g -Wall -pedantic -std=gnu99
 */

typedef struct MyObject {
  const char* name;
  unsigned data_size;
} MyObject;

static MyObject* my_object_new(const char* name, unsigned data_size)
{
  MyObject* retv = (MyObject*)malloc(sizeof(MyObject));
  retv->name = name;
  retv->data_size = data_size;
  return retv;
}

static const char* my_get_name(void* opaque)
{
  return ((MyObject*)opaque)->name;
}

static unsigned my_get_data_size(void* opaque)
{
  return ((MyObject*)opaque)->data_size;
}

XferObject my_object = {
  .get_name = my_get_name,
  .get_size = my_get_data_size,
  .read = 0,
  .write = 0
};

int main(int argc, char** argv)
{
  const char* hw = "hw";
  xfer_spaces_declare_object(hw, &my_object, my_object_new("top.dma.reg0", 4));
  xfer_spaces_declare_object(hw, &my_object, my_object_new("top.dma.reg1", 4));
  xfer_spaces_declare_object(hw, &my_object, my_object_new("top.eth.regA", 2));
  xfer_spaces_declare_object(hw, &my_object, my_object_new("top.eth.regB", 2));

  xfer_spaces_declare_object(hw, &my_object, my_object_new("top1.dma.reg0", 4));
  xfer_spaces_declare_object(hw, &my_object, my_object_new("top1.dma.reg1", 4));
  xfer_spaces_declare_object(hw, &my_object, my_object_new("top1.eth.regA", 2));
  xfer_spaces_declare_object(hw, &my_object, my_object_new("top1.eth.regB", 2));

  const char* xml = xfer_spaces_get_xml();
  printf("%s", xml);
  
  return 0;
}
