#include "DataManager.h"

#include <stdio.h>
#include <string.h>
#include "BankManager.h"
#include "data_ptrs.h"

#define MAX_PLAYER_SPRITE_SIZE 24

BankPtr bank_ptr;
UBYTE image_bank;
UBYTE collision_bank;
unsigned char* image_ptr;
UBYTE image_tile_width;
UBYTE image_tile_height;

void LoadTiles(UINT16 index) {
  UBYTE bank, size;
  UBYTE* data_ptr;

  SWITCH_ROM(5);

  // PUSH_BANK(DATA_PTRS_BANK);
  bank = tileset_bank_ptrs[index].bank;
  data_ptr = (UBYTE*)(tileset_bank_ptrs[index].offset + (BankDataPtr(bank)));
  // POP_BANK;

  // PUSH_BANK(bank);
  SWITCH_ROM(6);

  size = *(data_ptr++);
  set_bkg_data(0, size, data_ptr);
  // POP_BANK;
}

void LoadImage(UINT16 index) {
  UBYTE* data_ptr;

  SWITCH_ROM(5);

  // PUSH_BANK(DATA_PTRS_BANK);
  image_bank = background_bank_ptrs[index].bank;
  data_ptr = (UBYTE*)(background_bank_ptrs[index].offset + (BankDataPtr(image_bank)));
  // POP_BANK;

  // PUSH_BANK(image_bank);
  SWITCH_ROM(6);

  LoadTiles(*(data_ptr++));

  image_tile_width = *(data_ptr++);
  image_tile_height = *(data_ptr++);
  image_ptr = data_ptr;

  // POP_BANK;
}

void LoadScene(UINT16 index) {
  // static UBYTE bank;
  UBYTE bank;
  UBYTE* data_ptr;

  PUSH_BANK(DATA_PTRS_BANK);
  bank = scene_bank_ptrs[index].bank;
  data_ptr = (scene_bank_ptrs[index].offset + (BankDataPtr(bank)));
  collision_bank = collision_bank_ptrs[index].bank;
  POP_BANK;

  PUSH_BANK(bank);
  // LoadImage(1);
  LoadImage((*(data_ptr++) * 256) + *(data_ptr++));

  POP_BANK;
}
