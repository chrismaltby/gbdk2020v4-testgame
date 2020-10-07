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
  UBYTE i;
  // Init LCD
  LCDC_REG = 0x67;

  // Position Window Layer
  WX_REG = 7;
  WY_REG = MAXWNDPOSY + 1U;

  TestFn(0);

  PUSH_BANK(image_bank);

  for (i = 0; i != 18; i++)
  {
    set_bkg_tiles(0, i, 21, 1, image_ptr);
    image_ptr += image_tile_width;
  }

  POP_BANK;

  DISPLAY_ON;

  return 0;
}
