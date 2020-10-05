#include "DataManager.h"

#include <stdio.h>
#include <string.h>
#include "BankManager.h"
#include "Scroll.h"
#include "data_ptrs.h"

#define MAX_PLAYER_SPRITE_SIZE 24

BankPtr bank_ptr;
UBYTE image_bank;
UBYTE image_attr_bank;
UBYTE collision_bank;
unsigned char* image_ptr;
unsigned char* image_attr_ptr;
unsigned char* collision_ptr;
UBYTE image_tile_width;
UBYTE image_tile_height;
UINT16 image_width;
UINT16 image_height;
UBYTE sprites_len;
UBYTE actors_len;
UBYTE scene_type;
BankPtr scene_events_start_ptr;

void LoadTiles(UINT16 index) {
  UBYTE bank, size;
  UBYTE* data_ptr;

  PUSH_BANK(DATA_PTRS_BANK);
  bank = tileset_bank_ptrs[index].bank;
  data_ptr = (UBYTE*)(tileset_bank_ptrs[index].offset + (BankDataPtr(bank)));
  POP_BANK;

  PUSH_BANK(bank);
  size = *(data_ptr++);
  set_bkg_data(0, size, data_ptr);
  POP_BANK;
}

void LoadImage(UINT16 index) {
  UBYTE* data_ptr;

  PUSH_BANK(DATA_PTRS_BANK);
  image_bank = background_bank_ptrs[index].bank;
  data_ptr = (UBYTE*)(background_bank_ptrs[index].offset + (BankDataPtr(image_bank)));
  POP_BANK;

  PUSH_BANK(image_bank);

  LoadTiles(*(data_ptr++));

  image_tile_width = *(data_ptr++);
  image_tile_height = *(data_ptr++);
  image_width = image_tile_width * 8;
  image_height = image_tile_height * 8;
  image_ptr = data_ptr;

  POP_BANK;
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
  LoadImage((*(data_ptr++) * 256) + *(data_ptr++));

  POP_BANK;
}
