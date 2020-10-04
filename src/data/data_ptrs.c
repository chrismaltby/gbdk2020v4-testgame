#pragma bank 5
#include "data_ptrs.h"
#include "banks.h"

#ifdef __EMSCRIPTEN__
const unsigned int bank_data_ptrs[] = {
0,0,0,0,0,0,&bank_6_data,&bank_7_data
};
#endif

const BankPtr tileset_bank_ptrs[] = {
{0x06,0x13FB}
};

const BankPtr background_bank_ptrs[] = {
{0x06,0x184C},{0x06,0x19B7}
};

const BankPtr background_attr_bank_ptrs[] = {
{0x07,0x0000},{0x06,0x3AEB}
};

const BankPtr palette_bank_ptrs[] = {
{0x06,0x17DC},{0x06,0x17E4},{0x06,0x17EC},{0x06,0x181C},{0x06,0x184C}
};

const BankPtr sprite_bank_ptrs[] = {
{0x06,0x25FA},{0x06,0x277B},{0x06,0x27BC},{0x06,0x283D},{0x06,0x29BE},{0x06,0x29FF},{0x06,0x2AC0},{0x06,0x2B01},{0x06,0x2C82}
};

const BankPtr scene_bank_ptrs[] = {
{0x06,0x3C53},{0x06,0x3DBB}
};

const BankPtr collision_bank_ptrs[] = {
{0x06,0x2D43},{0x06,0x3983}
};

const BankPtr avatar_bank_ptrs[] = {
{0x00,0x0000}
};

const unsigned int music_tracks[] = {
0x4C72, 0x4C86, 0
};

const unsigned char music_banks[] = {
8, 8, 0
};

unsigned int start_scene_index = 0x0000;
int start_scene_x = 0x0098;
int start_scene_y = 0x0068;

char start_scene_dir_x = 1;
char start_scene_dir_y = 0;
unsigned int start_player_sprite = 0;
unsigned char start_player_move_speed = 1;
unsigned char start_player_anim_speed = 3;
unsigned char start_fade_style = 0;
unsigned char script_variables[500] = { 0 };
