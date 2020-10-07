#include "DataManager.h"
#include "BankManager.h"
#include "data_ptrs.h"

UBYTE *test1;

void TestFn(UINT16 index) {
  // static UBYTE bank;
  UBYTE bank;
  UBYTE* data_ptr;

  // PUSH_BANK(DATA_PTRS_BANK);
  SWITCH_ROM(5);

  bank = scene_bank_ptrs[index].bank;
  data_ptr = (scene_bank_ptrs[index].offset + 0x4000);
  collision_bank = collision_bank_ptrs[index].bank;
  // POP_BANK;

  SWITCH_ROM(bank);
  test1 = data_ptr;
  // POP_BANK;

  // PUSH_BANK(bank);
  // // LoadImage(1);
  // LoadImage((*(data_ptr++) * 256) + *(data_ptr++));

  // POP_BANK;
}


int main()
{
  TestFn(0);
  return 0;
}
