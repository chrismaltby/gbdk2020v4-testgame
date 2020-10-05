#include "Scroll.h"

#include "BankManager.h"
#include "Core_Main.h"
#include "DataManager.h"
#include "GameTime.h"
#include "FadeManager.h"

INT16 scroll_x = 0;
INT16 scroll_y = 0;
INT16 draw_scroll_x = 0;
UINT16 scroll_x_max = 0;
UINT16 scroll_y_max = 0;

INT16 scroll_offset_x = 0;

INT16 pending_h_x, pending_h_y;
UINT8 pending_h_i;
unsigned char *pending_h_map = 0;
unsigned char *pending_w_map = 0;
#ifdef CGB
unsigned char *pending_h_cmap = 0;
unsigned char *pending_w_cmap = 0;
#endif
INT16 pending_w_x, pending_w_y;
UINT8 pending_w_i;
Pos *scroll_target = 0;

void ScrollUpdateRow(INT16 x, INT16 y);
void RefreshScroll_b();

void ScrollUpdateRow(INT16 x, INT16 y)
{
  UINT8 i = 0u;
  UINT16 id;
  UBYTE screen_x, screen_y;
  unsigned char *map = image_ptr + image_tile_width * y + x;

  PUSH_BANK(image_bank);

  screen_x = x;
  screen_y = y;

  for (i = 0; i != 23; i++)
  {
    set_bkg_tiles(screen_x + i, screen_y, 1, 1, map++);
  }

  POP_BANK;
}

void RenderScreen()
{
  unsigned char *map;
  UINT8 i, temp;
  INT16 y;

  PUSH_BANK(image_bank);
  map = image_ptr;
  y = scroll_y >> 3;

  for (i = 0; i != 18; i++)
  {
    set_bkg_tiles(0, i, 21, 1, image_ptr);
    image_ptr += image_tile_width;
  }

  POP_BANK;

}
