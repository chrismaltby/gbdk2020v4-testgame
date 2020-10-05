
#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <gb/gb.h>

#define DATA_MANAGER_BANK 1

#define BankDataPtr(bank) ((UBYTE *)0x4000)

typedef struct _BankPtr {
  UBYTE bank;
  UWORD offset;
} BankPtr;

extern UBYTE image_bank;
extern UBYTE collision_bank;
extern unsigned char *image_ptr;
extern UBYTE image_tile_width;
extern UBYTE image_tile_height;

/**
 * Load tileset into VRAM
 * 
 * @param index tileset index in data_ptrs.h
 */
void LoadTiles(UINT16 index);

/**
 * Load image tiles into background
 * 
 * @param index image index in data_ptrs.h
 */
void LoadImage(UINT16 index);

/**
 * Load new scene including background image and all actors and triggers
 * 
 * @param index scene index in data_ptrs.h
 */
void LoadScene(UINT16 index);

#endif
