#include <gb/gb.h>

UBYTE *test1;
UBYTE test2;
UBYTE collision_bank;

typedef struct _BankPtr {
  UBYTE bank;
  UWORD offset;
} BankPtr;

const BankPtr scene_bank_ptrs1[] = {
{0x06,0x3C53},{0x06,0x3DBB}
};

const BankPtr collision_bank_ptrs1[] = {
{0x06,0x2D43},{0x06,0x3983}
};

void TestFn2(UBYTE i) {
  test2 = i;
}

void TestFn(UINT16 index) {
  // static UBYTE bank;
  UBYTE bank;
  UBYTE* data_ptr;

  bank = scene_bank_ptrs1[index].bank;
  data_ptr = (scene_bank_ptrs1[index].offset + 0x4000);
  collision_bank = collision_bank_ptrs1[index].bank;

  TestFn2(bank);
  test1 = data_ptr;
}


int main()
{
  TestFn(0);
  return 0;
}
