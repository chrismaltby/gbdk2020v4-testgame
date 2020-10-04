#include "Core_Main.h"

#include <gb/cgb.h>

#include "Actor.h"
#include "BankManager.h"
#include "Camera.h"
#include "DataManager.h"
#include "FadeManager.h"
#include "GameTime.h"
#include "Input.h"
#include "MusicManager.h"
#include "Palette.h"
#include "Projectiles.h"
#include "ScriptRunner.h"
#include "Scroll.h"
#include "Sprite.h"
#include "UI.h"
#include "gbt_player.h"
#include "data_ptrs.h"
#include "main.h"

UBYTE game_time;
UINT16 next_state;
UINT8 delta_time;
UINT16 current_state;
UINT8 state_running = 0;
UINT8 vbl_count;
INT16 old_scroll_x, old_scroll_y;
UINT8 music_mute_frames = 0;


const unsigned char earth_data[] = {

  /* Tile 0x00 */
  0x07,0x07,0x18,0x1F,0x32,0x2D,0x71,0x4E,0x70,0x4F,0xF8,0x87,0xF8,0x87,0xF8,0x87,
  0xFC,0x83,0xFE,0x81,0x7F,0x40,0x7F,0x40,0x3F,0x20,0x1F,0x18,0x07,0x07,0x00,0x00,
  0xC0,0xC0,0xF0,0x30,0x78,0x88,0x3C,0xC4,0x5C,0xA4,0x9E,0x62,0x3E,0xC2,0x3E,0xC2,
  0x5E,0xA2,0x7E,0x82,0x0C,0xF4,0x0C,0xF4,0x98,0x68,0xB0,0x70,0xC0,0xC0,0x00,0x00,
  0x07,0x07,0x1F,0x18,0x2F,0x30,0x4F,0x70,0x6F,0x50,0x9F,0xE0,0x9F,0xE0,0xBF,0xC0,
  0xFF,0x80,0xB7,0xC8,0x63,0x5C,0x43,0x7C,0x3F,0x20,0x1F,0x18,0x07,0x07,0x00,0x00,
  0xC0,0xC0,0xB0,0x70,0x18,0xE8,0x0C,0xF4,0x0C,0xF4,0x82,0x7E,0x82,0x7E,0x86,0x7A,
  0xC6,0x3A,0xE6,0x1A,0xF4,0x0C,0xFC,0x04,0xF8,0x08,0xF0,0x30,0xC0,0xC0,0x00,0x00,

  /* Tile 0x08 */
  0x07,0x07,0x1E,0x19,0x20,0x3F,0x40,0x7F,0x42,0x7D,0x81,0xFE,0x81,0xFE,0x83,0xFC,
  0xD7,0xA8,0xBB,0xC4,0x6E,0x51,0x7C,0x43,0x3F,0x20,0x1F,0x18,0x07,0x07,0x00,0x00,
  0xC0,0xC0,0x70,0xB0,0xE8,0x18,0xF4,0x0C,0xF4,0x0C,0xFE,0x02,0xFE,0x02,0xFE,0x02,
  0xFE,0x02,0x7E,0x82,0x3C,0xC4,0x3C,0xC4,0xF8,0x08,0xF0,0x30,0xC0,0xC0,0x00,0x00,
  0x07,0x07,0x1B,0x1C,0x20,0x3F,0x40,0x7F,0x40,0x7F,0xE0,0x9F,0x90,0xEF,0x89,0xF6,
  0x8D,0xF2,0x9F,0xE0,0x5E,0x61,0x6F,0x50,0x3F,0x20,0x1F,0x18,0x07,0x07,0x00,0x00,
  0xC0,0xC0,0xB0,0x70,0x28,0xD8,0x04,0xFC,0x2C,0xD4,0x1E,0xE2,0x1E,0xE2,0x3E,0xC2,
  0x7E,0x82,0xB6,0x4A,0xE4,0x1C,0xC4,0x3C,0xF8,0x08,0xF0,0x30,0xC0,0xC0,0x00,0x00,

  /* Tile 0x10 */
  0x07,0x07,0x18,0x1F,0x20,0x3F,0x40,0x7F,0x40,0x7F,0xEE,0x91,0xF1,0x8E,0xE0,0x9F,
  0xE0,0x9F,0xF1,0x8E,0x71,0x4E,0x72,0x4D,0x3F,0x20,0x1F,0x18,0x07,0x07,0x00,0x00,
  0xC0,0xC0,0xF0,0x30,0x08,0xF8,0x04,0xFC,0x04,0xFC,0x02,0xFE,0x02,0xFE,0x92,0x6E,
  0xD6,0x2A,0xFE,0x02,0xEC,0x14,0xFC,0x04,0xF8,0x08,0xF0,0x30,0xC0,0xC0,0x00,0x00,
  0x07,0x07,0x1D,0x1A,0x36,0x29,0x5C,0x63,0x6C,0x53,0xCE,0xB1,0x9F,0xE0,0x9E,0xE1,
  0xAE,0xD1,0xBF,0xC0,0x47,0x78,0x47,0x78,0x2F,0x30,0x1F,0x18,0x07,0x07,0x00,0x00,
  0xC0,0xC0,0x70,0xB0,0x08,0xF8,0x04,0xFC,0x04,0xFC,0xE2,0x1E,0x32,0xCE,0x0E,0xF2,
  0x0E,0xF2,0x1E,0xE2,0x1C,0xE4,0x2C,0xD4,0xF8,0x08,0xF0,0x30,0xC0,0xC0,0x00,0x00,

  /* Tile 0x18 */
  0x07,0x07,0x1E,0x19,0x33,0x2C,0x49,0x76,0x42,0x7D,0xC4,0xBB,0xC1,0xBE,0xC1,0xBE,
  0xE2,0x9D,0xF3,0x8C,0x78,0x47,0x78,0x47,0x3C,0x23,0x1C,0x1B,0x07,0x07,0x00,0x00,
  0xC0,0xC0,0x70,0xB0,0x68,0x98,0xC4,0x3C,0xC4,0x3C,0xEE,0x12,0xF2,0x0E,0xE2,0x1E,
  0xE2,0x1E,0xF2,0x0E,0x7C,0x84,0x7C,0x84,0xF8,0x08,0xF0,0x30,0xC0,0xC0,0x00,0x00
};

const unsigned char earth_tiles[] = {
  0x00,0x02,
  0x04,0x06,
  0x08,0x0A,
  0x0C,0x0E,
  0x10,0x12,
  0x14,0x16,
  0x18,0x1A
};

void SetScene(UINT16 state) {
  state_running = 0;
  next_state = state;
}

void vbl_update() {
  vbl_count++;

  // Update background scroll in vbl
  // interupt to prevent tearing
  SCX_REG = draw_scroll_x;
  SCY_REG = scroll_y;

#ifdef CGB
  if (palette_dirty) {
    set_bkg_palette(0, 8, BkgPaletteBuffer);
    set_sprite_palette(0, 8, SprPaletteBuffer);
    palette_dirty = FALSE;
  }
#endif

  if (music_mute_frames != 0) {
    music_mute_frames--;
    if (music_mute_frames == 0) {
      gbt_enable_channels(0xF);
    }
  }

  if (!hide_sprites) {
    SHOW_SPRITES;
  }
}

void lcd_update() {
  if (LYC_REG == 0x0) {
    if(WY_REG == 0x0) {
      HIDE_SPRITES;
    }
    
    // If UI is open cause lcd interupt
    // to fire again when first line of
    // window is being drawn
    if (WY_REG != MENU_CLOSED_Y) {
      LYC_REG = WY_REG;
    }
  } else if (WX_REG == WIN_LEFT_X) {
    // If window is covering entire scan line
    // can just hide all sprites until next frame
    HIDE_SPRITES;
    LYC_REG = 0x0;
  }
}

int core_start() {

#ifdef CGB
  if (_cpu == CGB_TYPE) {
    cpu_fast();
  }
#endif

  // Init LCD
  LCDC_REG = 0x67;

  // Set interupt handlers
  add_VBL(vbl_update);
  add_TIM(MusicUpdate);
  add_LCD(lcd_update);

// @todo - Check why this doesn't work
// #ifdef CGB 
//   TMA_REG = _cpu == CGB_TYPE ? 120U : 0xBCU;
// #else
//   TMA_REG = 0xBCU;
// #endif
//   TAC_REG = 0x04U;

  LYC_REG = 0x0;  // LCD interupt pos

  set_interrupts(VBL_IFLAG | TIM_IFLAG | LCD_IFLAG);
  enable_interrupts();

  STAT_REG = 0x45;

  // Set palettes
  BGP_REG = OBP0_REG = 0xE4U;
  OBP1_REG = 0xD2U;

  SCX_REG = 0;
  SCY_REG = 0;

  // Position Window Layer
  WX_REG = 7;
  WY_REG = MAXWNDPOSY + 1U;

  // Initialise Player
  player.sprite = 0;
  player.moving = TRUE;
  player.frame = 0;
  player.frames_len = 2;
  map_next_pos.x = start_scene_x;
  map_next_pos.y = start_scene_y;
  map_next_dir.x = player.dir.x = start_scene_dir_x;
  map_next_dir.y = player.dir.y = start_scene_dir_y;
  map_next_sprite = start_player_sprite;
  player.enabled = TRUE;
  player.move_speed = start_player_move_speed;
  player.anim_speed = start_player_anim_speed;
  fade_black = start_fade_style;

  state_running = 0;
  next_state = start_scene_index;
  game_time = 0;
  scene_type = 0;

  UIInit();
  FadeInit();
  ScriptRunnerInit();
  ActorsInit();

  /* Initialize the sprite */
  set_sprite_data(0x00, 0x1C, earth_data);
  set_sprite_prop(0, 0x00);
  set_sprite_prop(1, 0x00);
  set_sprite_tile(0, earth_tiles[0]);
  set_sprite_tile(1, earth_tiles[1]);
  move_sprite(0, 64, 64);
  move_sprite(1, 64 + 8, 64);

  // LoadScene(current_state);

  // DISPLAY_ON;
  // enable_interrupts();


  while (1) {
    while (state_running) {
    /* Game Core Loop Start *********************************/

      if (!vbl_count) {
        wait_vbl_done();
      }
      
    //   set_sprite_prop(0, 0x00);
    //   set_sprite_prop(1, 0x00);
    //   set_sprite_tile(0, earth_tiles[0]);
    //   set_sprite_tile(1, earth_tiles[1]);

      delta_time = vbl_count == 1u ? 0u : 1u;
      vbl_count = 0;

      last_joy = joy;
      joy = joypad();
      if ((joy & INPUT_DPAD) != (last_joy & INPUT_DPAD)) {
        recent_joy = joy & ~last_joy;
      }


      move_sprite(0, joy * 16, 16);
      move_sprite(1, (joy * 16) + 8, 16);    

    /* Game Core Loop End ***********************************/
    }

    // Fade out current scene
    FadeOut();
    while (fade_running) {
      wait_vbl_done();
      FadeUpdate();
    }
    if (!fade_black)
    {
      DISPLAY_OFF
    }

    state_running = TRUE;
    current_state = next_state;

    // Reset scroll target and camera
    scroll_target = 0;
    scroll_target = &camera_pos;
    camera_settings = CAMERA_LOCK_FLAG;

    // Disable timer script
    timer_script_duration = 0;

    UIInit();
    LoadScene(current_state);

    game_time = 0;
    old_scroll_x = scroll_x;
    old_scroll_y = scroll_y;

    // Fade in new scene
    DISPLAY_ON;
    FadeIn();

    // Run scene init script
    ScriptStart(&scene_events_start_ptr);
    ScriptRestoreCtx(0);

    UpdateCamera();
    RefreshScroll();
    UpdateActors();
    UIUpdate();

    // Wait for fade in to complete
    while (fade_running) {
      wait_vbl_done();
      FadeUpdate();
    }

  //   // set_sprite_tile(1, 4);
  //   // set_sprite_tile(2, 6);
  //   // set_sprite_tile(3, 8);
  //   // set_sprite_tile(4, 10);
  //   // set_sprite_tile(5, 4);

  //   // move_sprite(0, 16, 16);
  //   // move_sprite(1, 16, 16);    


  //   set_sprite_data(0x00, 0x1C, earth_data);

  }
}
