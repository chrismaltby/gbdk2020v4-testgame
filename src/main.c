#include <gb/gb.h>
#include <stdio.h>

unsigned char test1;
unsigned char test2;

typedef struct _BankPtr
{
  unsigned char bank;
  unsigned int offset;
} BankPtr;

const BankPtr scene_bank_ptrs[] = {{0x06, 0x3C53}};

void TestFn2(unsigned char i)
{
  test2 = i;
}

void TestFn(unsigned int index)
{
  // static unsigned char bank; // Switching bank to be static stops the bug from triggering
  unsigned char bank;
  unsigned int data_ptr;

  bank = scene_bank_ptrs[index].bank;
  data_ptr = scene_bank_ptrs[index].offset;

  /* This section doesn't matter but the bug doesn't trigger without it */
  test1 = 1;
  TestFn2(bank);
  /**/

  if (data_ptr == 0x3C53)
  {
    printf("OK\n");
  }
  else
  {
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
