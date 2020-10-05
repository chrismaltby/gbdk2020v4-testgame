#pragma bank 5

#include "states/PointNClick.h"
#include "Actor.h"
#include "Camera.h"
#include "DataManager.h"
#include "GameTime.h"
#include "Input.h"
#include "Trigger.h"

#define POINT_N_CLICK_CAMERA_DEADZONE 24

UBYTE last_hit_trigger = MAX_TRIGGERS;

void Start_PointNClick() {
  player.sprite_type = SPRITE_STATIC;
  player.dir.x = 0;
  player.dir.y = 1;
  player.rerender = TRUE;
}

void Update_PointNClick() {
  UBYTE tile_x, tile_y, hit_actor, hit_trigger, is_hover_actor, is_hover_trigger;

  tile_x = DIV_8(player.pos.x);
  tile_y = DIV_8(player.pos.y);

  player.moving = FALSE;
  player.dir.x = 0;
  player.dir.y = 0;

  // Move cursor horizontally
  if (INPUT_LEFT && Gt16(player.pos.x, 0)) {
    player.dir.x = -1;
    player.moving = TRUE;
  } else if (INPUT_RIGHT && Lt16(player.pos.x, image_width - 8)) {
    player.dir.x = 1;
    player.moving = TRUE;
  }

  // Move cursor vertically
  if (INPUT_UP && Gt16(player.pos.y, 8)) {
    player.dir.y = -1;
    player.moving = TRUE;
  } else if (INPUT_DOWN && Lt16(player.pos.y, image_height)) {
    player.dir.y = 1;
    player.moving = TRUE;
  }

  // Find trigger or actor under player cursor
  hit_trigger = TriggerAtTile(tile_x, tile_y - 1);
  hit_actor = ActorAtTile(tile_x, tile_y, TRUE);

  is_hover_trigger = (hit_trigger != NO_TRIGGER_COLLISON) && (hit_trigger != last_hit_trigger) &&
                     (triggers[hit_trigger].events_ptr.bank != 0);
  is_hover_actor = (hit_actor != NO_ACTOR_COLLISON) && (hit_actor != 0) &&
                   (actors[hit_actor].events_ptr.bank != 0);

  // Set player cursor to second frame on hover
  if ((is_hover_trigger || is_hover_actor) && player.frames_len != 1) {
    player.frame = 1;
    player.rerender = TRUE;
  } else {
    player.frame = 0;
    player.rerender = TRUE;
  }

  if (INPUT_A_PRESSED) {
    player.moving = FALSE;

    if (is_hover_actor) {
      // Run actor's interact script
      ActorRunScript(hit_actor);
    } else if (is_hover_trigger) {
      // Run trigger script
      TriggerRunScript(hit_trigger);
    }
  }

}
