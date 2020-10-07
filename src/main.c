#include <gb/gb.h>
#include <stdio.h>

UWORD test1;
UBYTE test2;
UBYTE test3;

typedef struct _BankPtr
{
  UBYTE bank;
  UWORD offset;
} BankPtr;

const BankPtr scene_bank_ptrs[] = {
    {0x06, 0x3C53}};

void TestFn2(UBYTE i)
{
  test2 = i;
}

void TestFn(UINT16 index)
{
  // static UBYTE bank;
  UBYTE bank;
  UWORD data_ptr;

  bank = scene_bank_ptrs[index].bank;
  data_ptr = scene_bank_ptrs[index].offset;

  /* This section doesn't matter but bug doesn't trigger without it */
  test3 = 1;
  TestFn2(bank);
  test1 = data_ptr;
  /**/

  if (data_ptr == 0x3C53) {
    printf("OK\n");
  } else {
    printf("FAIL\n");
  }
}

int main()
{
  LCDC_REG = 0x67;
  BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;
  WX_REG = MAXWNDPOSX;
  WY_REG = MAXWNDPOSY;

  TestFn(0);

  return 0;
}
