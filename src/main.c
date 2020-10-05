#include "DataManager.h"
#include "BankManager.h"

int main()
{
  UBYTE i;
  // Init LCD
  LCDC_REG = 0x67;

  // Position Window Layer
  WX_REG = 7;
  WY_REG = MAXWNDPOSY + 1U;

  LoadScene(0);

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
