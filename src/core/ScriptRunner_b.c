#pragma bank 4

#include "ScriptRunner.h"
#include "BankData.h"
#include "BankManager.h"
#include "DataManager.h"
#include "UI.h"
#include "Math.h"
#include "Actor.h"
#include "GameTime.h"
#include "Core_Main.h"
#include "Collision.h"
#include "Input.h"
#include "Sprite.h"
#include "Scroll.h"
#include "data_ptrs.h"
#include "Projectiles.h"
#include "states/Platform.h"
#include <rand.h>

#define RAM_START_PTR 0xA000
#define RAM_START_VARS_PTR 0xA0FF

#define EMOTE_SPRITE 124
#define BUBBLE_ANIMATION_FRAMES 15
#define BUBBLE_TOTAL_FRAMES 60

void ScriptHelper_CalcDest();
void ScriptHelper_ClampCamDest();

UBYTE* RAMPtr;
UBYTE scene_stack_ptr = 0;
SCENE_STATE scene_stack[MAX_SCENE_STATES] = {{0}};
UBYTE emote_timer = 0;
UBYTE shake_time = 0;
UBYTE after_lock_camera = FALSE;
Actor* tmp_actor;

const BYTE emote_offsets[] = {2, 1, 0, -1, -2, -3, -4, -5, -6, -5, -4, -3, -2, -1, 0, 0, 0, 0, 0,
                              0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0,
                              0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0};

const SCRIPT_CMD script_cmds[] = {
    {Script_End_b, 0},                 // 0x00
    {Script_Text_b, 3},                // 0x01
    {Script_Goto_b, 2},                // 0x02
    {Script_IfFlag_b, 4},              // 0x03
    {Script_Noop_b, 0},                // 0x04
    {Script_SetFlag_b, 2},             // 0x05
    {Script_ClearFlag_b, 2},           // 0x06
    {Script_ActorSetDir_b, 1},         // 0x07
    {Script_ActorActivate_b, 1},       // 0x08
    {Script_CameraMoveTo_b, 3},        // 0x09
    {Script_CameraLock_b, 1},          // 0x0A
    {Script_Wait_b, 1},                // 0x0B
    {Script_FadeOut_b, 1},             // 0x0C
    {Script_FadeIn_b, 1},              // 0x0D
    {Script_LoadScene_b, 6},           // 0x0E
    {Script_ActorSetPos_b, 2},         // 0x0F
    {Script_ActorMoveTo_b, 4},         // 0x10
    {Script_ShowSprites_b, 0},         // 0x11
    {Script_HideSprites_b, 0},         // 0x12
    {Script_PlayerSetSprite_b, 3},     // 0x13
    {Script_ActorShow_b, 0},           // 0x14
    {Script_ActorHide_b, 0},           // 0x15
    {Script_ActorSetEmote_b, 1},       // 0x16
    {Script_CameraShake_b, 1},         // 0x17
    {Script_Noop_b, 0},                // 0x18
    {Script_ShowOverlay_b, 3},         // 0x19
    {Script_HideOverlay_b, 0},         // 0x1A
    {Script_Noop_b, 0},                // 0x1B
    {Script_OverlayMoveTo_b, 3},       // 0x1C
    {Script_AwaitInput_b, 1},          // 0x1D
    {Script_MusicPlay_b, 2},           // 0x1E
    {Script_MusicStop_b, 0},           // 0x1F
    {Script_ResetVariables_b, 0},      // 0x20
    {Script_NextFrame_b, 0},           // 0x21
    {Script_IncFlag_b, 2},             // 0x22
    {Script_DecFlag_b, 2},             // 0x23
    {Script_SetFlagValue_b, 3},        // 0x24
    {Script_IfValue_b, 6},             // 0x25
    {Script_IfInput_b, 3},             // 0x26
    {Script_Choice_b, 5},              // 0x27
    {Script_ActorPush_b, 1},           // 0x28
    {Script_IfActorPos_b, 4},          // 0x29
    {Script_LoadData_b, 0},            // 0x2A
    {Script_SaveData_b, 0},            // 0x2B
    {Script_ClearData_b, 0},           // 0x2C
    {Script_IfSavedData_b, 2},         // 0x2D
    {Script_IfActorDirection_b, 3},    // 0x2E
    {Script_SetFlagRandomValue_b, 4},  // 0x2F
    {Script_ActorGetPos_b, 0},         // 0x30
    {Script_ActorSetPosToVal_b, 0},    // 0x31
    {Script_ActorMoveToVal_b, 2},      // 0x32
    {Script_ActorMoveRel_b, 6},        // 0x33
    {Script_ActorSetPosRel_b, 4},      // 0x34
    {Script_MathAdd_b, 3},             // 0x35
    {Script_MathSub_b, 3},             // 0x36
    {Script_MathMul_b, 3},             // 0x37
    {Script_MathDiv_b, 3},             // 0x38
    {Script_MathMod_b, 3},             // 0x39
    {Script_MathAddVal_b, 1},          // 0x3A
    {Script_MathSubVal_b, 1},          // 0x3B
    {Script_MathMulVal_b, 0},          // 0x3C
    {Script_MathDivVal_b, 0},          // 0x3D
    {Script_MathModVal_b, 0},          // 0x3E
    {Script_CopyVal_b, 0},             // 0x3F
    {Script_IfValueCompare_b, 3},      // 0x40
    {Script_LoadVectors_b, 4},         // 0x41
    {Script_ActorSetMoveSpeed_b, 1},   // 0x42
    {Script_ActorSetAnimSpeed_b, 1},   // 0x43
    {Script_TextSetAnimSpeed_b, 4},    // 0x44
    {Script_ScenePushState_b, 0},      // 0x45
    {Script_ScenePopState_b, 1},       // 0x46
    {Script_ActorInvoke_b, 0},         // 0x47
    {Script_StackPush_b, 0},           // 0x48
    {Script_StackPop_b, 0},            // 0x49
    {Script_SceneResetStack_b, 0},     // 0x4A
    {Script_ScenePopAllState_b, 1},    // 0x4B
    {Script_SetInputScript_b, 5},      // 0x4C
    {Script_RemoveInputScript_b, 1},   // 0x4D
    {Script_ActorSetFrame_b, 1},       // 0x4E
    {Script_ActorSetFlip_b, 1},        // 0x4F
    {Script_TextMulti_b, 1},           // 0x50
    {Script_ActorSetFrameToVal_b, 2},  // 0x51
    {Script_VariableAddFlags_b, 3},    // 0x52
    {Script_VariableClearFlags_b, 3},  // 0x53
    {Script_SoundPlayTone_b, 3},       // 0x54
    {Script_SoundStopTone_b, 0},       // 0x55
    {Script_SoundPlayBeep_b, 1},       // 0x56
    {Script_SoundPlayCrash_b, 0},      // 0x57
    {Script_SetTimerScript_b, 4},      // 0x58
    {Script_ResetTimer_b, 0},          // 0x59
    {Script_RemoveTimerScript_b, 0},   // 0x5A
    {Script_TextWithAvatar_b, 4},      // 0x5B
    {Script_TextMenu_b, 7},            // 0x5C
    {Script_ActorSetCollisions_b, 1},  // 0x5D
    {Script_LaunchProjectile_b, 5},    // 0x5E
    {Script_SetFlagProperty_b, 4},     // 0x5F
    {Script_ActorSetSprite_b, 2},      // 0x60
    {Script_IfActorRelActor_b, 4},     // 0x61
    {Script_PlayerBounce_b, 1},        // 0x62
    {Script_WeaponAttack_b, 2},        // 0x63
    {Script_PalSetBackground_b, 2},    // 0x64
    {Script_PalSetSprite_b, 2},        // 0x65
    {Script_PalSetUI_b, 2},            // 0x66
    {Script_ActorStopUpdate_b, 0},     // 0x67
    {Script_ActorSetAnimate_b, 1},     // 0x68
    {Script_IfColorSupported_b, 2},    // 0x69
    {Script_FadeSetSettings_b, 1},     // 0x6A
};

void ScriptTimerUpdate_b() {
  // Don't update timer while script is running
  if (active_script_ctx.script_ptr != 0) {
    return;
  }

  // Check if timer is enabled
  if (timer_script_duration != 0) {
    if (timer_script_time == 0) {
      last_joy = last_joy & 0xF0;

      ScriptStartBg(&timer_script_ptr, 255);

      // Reset the countdown timer
      timer_script_time = timer_script_duration;
    }
  }
}

UBYTE ScriptUpdate_MoveActor() {
  BYTE new_dir_x = 0;
  BYTE new_dir_y = 0;
  return FALSE;
}

UBYTE ScriptUpdate_MoveActorDiag() {
  BYTE new_dir_x = 0;
  BYTE new_dir_y = 0;

  return FALSE;
}

UBYTE ScriptUpdate_AwaitFade() {
  return TRUE;
}

UBYTE ScriptUpdate_AwaitUIClosed() {
  return UIIsClosed();
}

UBYTE ScriptUpdate_AwaitUIAtDest() {
  return UIAtDest();
}

UBYTE ScriptUpdate_AwaitInputPres() {
  return (joy != last_joy && joy & await_input) != 0;
}

UBYTE ScriptUpdate_Wait() {
  if (active_script_ctx.wait_time == 0) {
    return TRUE;
  }
  active_script_ctx.wait_time--;
  return FALSE;
}

UBYTE ScriptUpdate_Emote() {
  UINT16 screen_x;
  UINT16 screen_y;

  if (emote_timer == BUBBLE_TOTAL_FRAMES) {
    move_sprite(0, 0, 0);
    move_sprite(1, 0, 0);    
    return TRUE;
  } else {
    if (emote_timer < BUBBLE_ANIMATION_FRAMES) {
      screen_y += emote_offsets[emote_timer];
    }

    move_sprite(0, screen_x, screen_y - 16u);
    move_sprite(1, screen_x + 8u, screen_y - 16u);

    emote_timer++;
    return FALSE;
  }
}

UBYTE ScriptUpdate_MoveCamera() {
  return FALSE;
}

UBYTE ScriptUpdate_CamShake() {
  if (shake_time == 0) {
    scroll_offset_x = 0;
    return TRUE;
  }

  shake_time--;

  // Handle Shake
  if (shake_time != 0) {
    scroll_offset_x = (INT16)(shake_time & 0x5);
  }

  return FALSE;
}

/*
 * Command: Noop
 * ----------------------------
 * Perform no action, jump ahead by size of command.
 * Useful for skipping unimplemented commands.
 */
void Script_Noop_b() {}

/*
 * Command: End
 * ----------------------------
 * Stop current script from running and reset script pointer.
 */
void Script_End_b() {
  active_script_ctx.script_ptr_bank = 0;
  active_script_ctx.script_ptr = 0;
}

/*
 * Command: Text
 * ----------------------------
 * Display a line of dialogue.
 *
 *   arg0: High 8 bits for string index
 *   arg1: Low 8 bits for string index
 */
void Script_Text_b() {
  UIShowText(script_cmd_args[0], (script_cmd_args[1] * 256) + script_cmd_args[2]);
  active_script_ctx.script_update_fn = ScriptUpdate_AwaitUIClosed;
}

/*
 * Command: Goto
 * ----------------------------
 * Jump to new script pointer position.
 *
 *   arg0: High 8 bits for new pointer
 *   arg1: Low 8 bits for new pointer
 */
void Script_Goto_b() {
  active_script_ctx.script_ptr = active_script_ctx.script_start_ptr + (script_cmd_args[0] * 256) + script_cmd_args[1];
}

/*
 * Command: IfFlag
 * ----------------------------
 * Jump to new script pointer position if specified flag is not false.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: High 8 bits for new pointer
 *   arg3: Low 8 bits for new pointer
 */
void Script_IfFlag_b() {
  if (script_variables[(script_cmd_args[0] * 256) +
                       script_cmd_args[1]]) {  // True path, jump to position specified by ptr
    active_script_ctx.script_ptr = active_script_ctx.script_start_ptr + (script_cmd_args[2] * 256) + script_cmd_args[3];
  }
}

/*
 * Command: IfValue
 * ----------------------------
 * Jump to new script pointer position if specified flag is true when compared using operator to
 * comparator.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Operator
 *   arg3: Comparator
 *   arg4: High 8 bits for new pointer
 *   arg5: Low 8 bits for new pointer
 */
void Script_IfValue_b() {
  UBYTE value, match;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  value = script_variables[ptr];

  switch (script_cmd_args[2]) {
    case OPERATOR_EQ:
      match = value == script_cmd_args[3];
      break;
    case OPERATOR_LT:
      match = value < script_cmd_args[3];
      break;
    case OPERATOR_LTE:
      match = value <= script_cmd_args[3];
      break;
    case OPERATOR_GT:
      match = value > script_cmd_args[3];
      break;
    case OPERATOR_GTE:
      match = value >= script_cmd_args[3];
      break;
    case OPERATOR_NE:
      match = value != script_cmd_args[3];
      break;
    default:
      match = FALSE;
  }

  if (match) {  // True path, jump to position specified by ptr
    active_script_ctx.script_ptr = active_script_ctx.script_start_ptr + (script_cmd_args[4] * 256) + script_cmd_args[5];
  }
}

/*
 * Command: SetFlag
 * ----------------------------
 * Set specified flag to be true.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 */
void Script_SetFlag_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  script_variables[ptr] = TRUE;
}

/*
 * Command: ClearFlag
 * ----------------------------
 * Set specified flag to be false.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 */
void Script_ClearFlag_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  script_variables[ptr] = FALSE;
}

/*
 * Command: ActorSetDir
 * ----------------------------
 * Set active actor facing direction.
 *
 *   arg0: Direction for active actor to face
 */
void Script_ActorSetDir_b() {
}

/*
 * Command: ActorActivate
 * ----------------------------
 * Set active actor.
 *
 *   arg0: Actor index
 */
void Script_ActorActivate_b() {
}

/*
 * Command: CameraMoveTo
 * ----------------------------
 * Move camera to specified locatopn.
 *
 *   arg0: Camera destination X (in Tiles)
 *   arg1: Camera destination Y (in Tiles)
 *   arg2: Camera settings
 */
void Script_CameraMoveTo_b() {
}

/*
 * Command: CameraLock
 * ----------------------------
 * Move camera to specified locatopn.
 *
 *   arg0: Camera settings
 */
void Script_CameraLock_b() {
  ScriptHelper_ClampCamDest();
}

void ScriptHelper_ClampCamDest() {
}

/*
 * Command: Wait
 * ----------------------------
 * Wait for a specified number of frames before continuing script.
 *
 *   arg0: Frames to wait
 */
void Script_Wait_b() {
  active_script_ctx.wait_time = script_cmd_args[0];
  active_script_ctx.script_update_fn = ScriptUpdate_Wait;
}

/*
 * Command: FadeOut
 * ----------------------------
 * Fade screen to white.
 *
 *   arg0: Fade speed
 */
void Script_FadeOut_b() {
}

/*
 * Command: FadeIn
 * ----------------------------
 * Fade screen from white.
 *
 *   arg0: Fade speed
 */
void Script_FadeIn_b() {
}

/*
 * Command: FadeSetSettings
 * ----------------------------
 * Set Fade settings.
 *
 *   arg0: Fade style (0=white, 1=black)
 */
void Script_FadeSetSettings_b() {
}

/*
 * Command: LoadScene
 * ----------------------------
 * Load a new scene.
 *
 *   arg0: High 8 bits for stage index
 *   arg1: Low 8 bits for stage index
 *   arg2: Starting X Pos
 *   arg3: Starting Y Pos
 *   arg4: Starting direction
 *   arg5: Fade speed
 */
void Script_LoadScene_b() {
  UINT16 scene_next_index = (script_cmd_args[0] * 256) + script_cmd_args[1];
}

/*
 * Command: ActorSetPos
 * ----------------------------
 * Instantly position actor at new location.
 *
 *   arg0: New X Pos
 *   arg1: New Y Pos
 */
void Script_ActorSetPos_b() {
}

/*
 * Command: ActorMoveTo
 * ----------------------------
 * Walk actor to new location.
 *
 *   arg0: New X Pos
 *   arg1: New Y Pos
 */
void Script_ActorMoveTo_b() {  
}

void ScriptHelper_CalcDest() {
}

/*
 * Command: ShowSprites
 * ----------------------------
 * Make all sprites visible
 */
void Script_ShowSprites_b() {
  hide_sprites = FALSE;
  SHOW_SPRITES;
}

/*
 * Command: HideSprites
 * ----------------------------
 * Hide all sprites
 */
void Script_HideSprites_b() {
  hide_sprites = TRUE;
  HIDE_SPRITES;
}

/*
 * Command: ActorShow
 * ----------------------------
 * Unhide actor
 */
void Script_ActorShow_b() {
}

/*
 * Command: ActorHide
 * ----------------------------
 * Hide actor
 */
void Script_ActorHide_b() {
}

/*
 * Command: ActorSetCollisions
 * ----------------------------
 * Set collisions enabled flag for actor.
 *
 *   arg0: Enabled
 */
void Script_ActorSetCollisions_b() {
}

/*
 * Command: ActorSetEmote
 * ----------------------------
 * Display emote bubble over actor.
 *
 *   arg0: Emote Id
 */
void Script_ActorSetEmote_b() {
}

/*
 * Command: CameraShake
 * ----------------------------
 * Shake camera for earthquake effect.
 *
 *   arg0: Number of frames to shake for
 */
void Script_CameraShake_b() {
  shake_time = script_cmd_args[0];
  active_script_ctx.script_update_fn = ScriptUpdate_CamShake;
}

/*
 * Command: ShowOverlay
 * ----------------------------
 * Load image into window buffer and position.
 */
void Script_ShowOverlay_b() {
  UISetColor(script_cmd_args[0]);
  UISetPos(script_cmd_args[1] << 3, script_cmd_args[2] << 3);
}

/*
 * Command: HideOverlay
 * ----------------------------
 * Hide window buffer
 */
void Script_HideOverlay_b() {
  UISetPos(0, MENU_CLOSED_Y);
}

/*
 * Command: OverlaySetPos
 * ----------------------------
 * Window buffer set position to X/Y
 */
void Script_OverlaySetPos_b() {
  UISetPos(script_cmd_args[0] << 3, script_cmd_args[1] << 3);
}

/*
 * Command: OverlaySetPos
 * ----------------------------
 * Window buffer move position to X/Y with speed
 */
void Script_OverlayMoveTo_b() {
  UIMoveTo(script_cmd_args[0] << 3, script_cmd_args[1] << 3, script_cmd_args[2]);
  active_script_ctx.script_update_fn = ScriptUpdate_AwaitUIAtDest;
}

/*
 * Command: AwaitInput
 * ----------------------------
 * Pause script until joy overlaps bits with provided input
 */
void Script_AwaitInput_b() {
  await_input = script_cmd_args[0];
  active_script_ctx.script_update_fn = ScriptUpdate_AwaitInputPres;
}

/*
 * Command: MusicPlay
 * ----------------------------
 * Play the music track with given index
 */
void Script_MusicPlay_b() {
}

/*
 * Command: MusicStop
 * ----------------------------
 * Stop any playing music
 */
void Script_MusicStop_b() {
}

/*
 * Command: ResetVariables
 * ----------------------------
 * Reset all variables back to false
 */
void Script_ResetVariables_b() {
  UWORD i;
  for (i = 0; i != NUM_VARIABLES; ++i) {
    script_variables[i] = FALSE;
  }
}

/*
 * Command: NextFrame
 * ----------------------------
 * Wait until next frame
 */
void Script_NextFrame_b() {
  active_script_ctx.script_await_next_frame = TRUE;
}

/*
 * Command: IncFlag
 * ----------------------------
 * Increase value stored in flag
 */
void Script_IncFlag_b() {
  UBYTE value;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  value = script_variables[ptr];
  if (value != 255) {
    script_variables[ptr] = value + 1;
  }
}

/*
 * Command: DecFlag
 * ----------------------------
 * Decrease value stored in flag
 */
void Script_DecFlag_b() {
  UBYTE value;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  value = script_variables[ptr];
  if (value != 0) {
    script_variables[ptr] = value - 1;
  }
}

/*
 * Command: SetFlagValue
 * ----------------------------
 * Set flag to specific value
 */
void Script_SetFlagValue_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  script_variables[ptr] = script_cmd_args[2];
}

/*
 * Command: SetFlagProperty
 * ----------------------------
 * Set flag to an actor property
 */
void Script_SetFlagProperty_b() {
}

/*
 * Command: IfInput
 * ----------------------------
 * Goto true path if joy mask overlaps
 */
void Script_IfInput_b() {
  UBYTE mask;
  mask = 0;
  mask = script_cmd_args[0];
  if ((joy & mask) != 0) {  // True path, jump to position specified by ptr
    active_script_ctx.script_ptr = active_script_ctx.script_start_ptr + (script_cmd_args[1] * 256) + script_cmd_args[2];
  }
}

/*
 * Command: Choice
 * ----------------------------
 * Display multiple choice input
 */
void Script_Choice_b() {
  UIShowChoice((script_cmd_args[0] * 256) + script_cmd_args[1], script_cmd_args[2],
               (script_cmd_args[3] * 256) + script_cmd_args[4]);
  active_script_ctx.script_update_fn = ScriptUpdate_AwaitUIClosed;
}

/*
 * Command: Menu
 * ----------------------------
 * Display multiple choice menu
 */
void Script_TextMenu_b() {
  UIShowMenu((script_cmd_args[0] * 256) + script_cmd_args[1], script_cmd_args[2],
             (script_cmd_args[3] * 256) + script_cmd_args[4], script_cmd_args[5],
             script_cmd_args[6]);
  active_script_ctx.script_update_fn = ScriptUpdate_AwaitUIClosed;
}

/*
 * Command: PlayerBounce
 * ----------------------------
 * Cause player to bounce (Platform scenes only)
 */
void Script_PlayerBounce_b() {
}

/*
 * Command: ActorSetSprite
 * ----------------------------
 * Change sprite used by player
 */
void Script_ActorSetSprite_b() {
}

/*
 * Command: PlayerSetSprite
 * ----------------------------
 * Change sprite used by player
 */
void Script_PlayerSetSprite_b() {
  UWORD sprite_index;

  // Load Player Sprite
  sprite_index = (script_cmd_args[0] * 256) + script_cmd_args[1];


}

/*
 * Command: ActorPush
 * ----------------------------
 * Push actor in direction player is facing
 */
void Script_ActorPush_b() {
  UINT16 dest_x, dest_y;
}

/*
 * Command: IfActorPos
 * ----------------------------
 * Jump to new script pointer position if specified actor is at desired position.
 *
 *   arg0: Actor index
 *   arg1: Actor X Pos
 *   arg2: Actor Y Pos
 *   arg3: High 8 bits for new pointer
 *   arg4: Low 8 bits for new pointer
 */
void Script_IfActorPos_b() {
  UBYTE tile_x, tile_y;
}

/*
 * Command: SaveData
 * ----------------------------
 * Store current scene, player position and direction, current sprite and variable values into RAM
 */
void Script_SaveData_b() {
  UWORD i;

  ENABLE_RAM;

  RAMPtr = (UBYTE*)RAM_START_PTR;
  RAMPtr[0] = TRUE;  // Flag to determine if data has been stored


  // Save variable values
  RAMPtr = (UBYTE*)RAM_START_VARS_PTR;
  for (i = 0; i < NUM_VARIABLES; i++) {
    RAMPtr[i] = script_variables[i];
  }

  DISABLE_RAM;
}

/*
 * Command: LoadData
 * ----------------------------
 * Restore current scene, player position and direction, current sprite and variable values from RAM
 */
void Script_LoadData_b() {
  UINT16 scene_next_index;
  UWORD i;

  ENABLE_RAM;

  RAMPtr = (UBYTE*)RAM_START_PTR;
  if (*RAMPtr == TRUE) {
    // Set scene index
    RAMPtr++;
    scene_next_index = (UWORD)((*(RAMPtr++)) * 256) + *RAMPtr;



    // Load player sprite
    RAMPtr++;

    // Load variable values
    RAMPtr = (UBYTE*)RAM_START_VARS_PTR;
    for (i = 0; i < NUM_VARIABLES; i++) {
      script_variables[i] = RAMPtr[i];
    }

  }

  DISABLE_RAM;
}

/*
 * Command: ClearData
 * ----------------------------
 * Clear current data in RAM
 */
void Script_ClearData_b() {
  ENABLE_RAM;
  RAMPtr = (UBYTE*)RAM_START_PTR;
  RAMPtr[0] = FALSE;
  DISABLE_RAM;
}

/*
 * Command: IfSavedData
 * ----------------------------
 * Jump to new script pointer position if data is saved in RAM.
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 */
void Script_IfSavedData_b() {
  UBYTE jump;

  ENABLE_RAM;
  RAMPtr = (UBYTE*)RAM_START_PTR;
  jump = 0;
  jump = *RAMPtr == TRUE;
  DISABLE_RAM;

  if (jump) {  // True path, jump to position specified by ptr
    active_script_ctx.script_ptr = active_script_ctx.script_start_ptr + (script_cmd_args[0] * 256) + script_cmd_args[1];
  }
}

/*
 * Command: IfActorDirection
 * ----------------------------
 * Jump to new script pointer position if actor direction matches.
 *
 *   arg0: Actor index
 *   arg1: Direction for active actor to match
 *   arg2: High 8 bits for new pointer
 *   arg3: Low 8 bits for new pointer
 */
void Script_IfActorDirection_b() {
}

/*
 * Command: IfActorRelActor
 * ----------------------------
 * Jump to new script pointer position if actor position is in specificied direction relative to
 * second actor.
 *
 *   arg0: Operation
 *   arg1: Second actor index
 *   arg2: High 8 bits for new pointer
 *   arg3: Low 8 bits for new pointer
 */
void Script_IfActorRelActor_b() {
}

/*
 * Command: SetFlagRandomValue
 * ----------------------------
 * Set flag to random value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Max value
 */
void Script_SetFlagRandomValue_b() {
  UBYTE rand_val;
  UBYTE modulo;
  UBYTE offset;
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  rand_val = rand();
  offset = script_cmd_args[2];
  modulo = script_cmd_args[3] + 1;
  script_variables[ptr] = offset + (rand_val % modulo);
}

/*
 * Command: ActorGetPos
 * ----------------------------
 * Store Actor position in variables
 */
void Script_ActorGetPos_b() {
}

/*
 * Command: ActorSetPosToVal
 * ----------------------------
 * Set Actor position from variables
 */
void Script_ActorSetPosToVal_b() {

}

/*
 * Command: ActorMoveToVal
 * ----------------------------
 * Set Actor position from variables
 */
void Script_ActorMoveToVal_b() {  
}

/*
 * Command: ActorMoveRel
 * ----------------------------
 * Walk actor to relative location.
 *
 *   arg0: Offset X Pos
 *   arg1: Offset Y Pos
 */
void Script_ActorMoveRel_b() {
}

/*
 * Command: ActorSetPosRel
 * ----------------------------
 * Instantly position actor at relative location.
 *
 *   arg0: Offset X Pos
 *   arg1: Offset Y Pos
 */
void Script_ActorSetPosRel_b() {
}

/*
 * Command: MathAdd
 * ----------------------------
 * Add value to flag
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathAdd_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a + b;
}

/*
 * Command: MathSub
 * ----------------------------
 * Subtract value from flag
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathSub_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a - b;
}

/*
 * Command: MathMul
 * ----------------------------
 * Multiply flag by value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathMul_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a * b;
}

/*
 * Command: MathDiv
 * ----------------------------
 * Divide flag by value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathDiv_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a / b;
}

/*
 * Command: MathMod
 * ----------------------------
 * Flag modulo by value
 *
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_MathMod_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a % b;
}

/*
 * Command: MathAddVal
 * ----------------------------
 * Add value from flag to flag
 */
void Script_MathAddVal_b() {
  UBYTE a = script_variables[active_script_ctx.script_ptr_x];
  UBYTE b = script_variables[active_script_ctx.script_ptr_y];
  UBYTE clamp = script_cmd_args[0];

  if (!clamp || a < 255 - b) {
    script_variables[active_script_ctx.script_ptr_x] = a + b;
  } else {
    script_variables[active_script_ctx.script_ptr_x] = 255;
  }
  
}

/*
 * Command: MathSubVal
 * ----------------------------
 * Subtract value from flag to flag
 */
void Script_MathSubVal_b() {
  UBYTE a = script_variables[active_script_ctx.script_ptr_x];
  UBYTE b = script_variables[active_script_ctx.script_ptr_y];
  UBYTE clamp = script_cmd_args[0];

  if (!clamp || a > b) {
    script_variables[active_script_ctx.script_ptr_x] = a - b;
  } else {
    script_variables[active_script_ctx.script_ptr_x] = 0;
  }
}

/*
 * Command: MathMulVal
 * ----------------------------
 * Multiply value from flag to flag
 */
void Script_MathMulVal_b() {
  UBYTE a = script_variables[active_script_ctx.script_ptr_x];
  UBYTE b = script_variables[active_script_ctx.script_ptr_y];
  script_variables[active_script_ctx.script_ptr_x] = a * b;
}

/*
 * Command: MathDiv
 * ----------------------------
 * Divide value from flag to flag
 */
void Script_MathDivVal_b() {
  UBYTE a = script_variables[active_script_ctx.script_ptr_x];
  UBYTE b = script_variables[active_script_ctx.script_ptr_y];
  script_variables[active_script_ctx.script_ptr_x] = a / b;
}

/*
 * Command: MathModVal
 * ----------------------------
 * Modulo value from flag to flag
 */
void Script_MathModVal_b() {
  UBYTE a = script_variables[active_script_ctx.script_ptr_x];
  UBYTE b = script_variables[active_script_ctx.script_ptr_y];
  script_variables[active_script_ctx.script_ptr_x] = a % b;
}

/*
 * Command: CopyVal
 * ----------------------------
 * Copy value from flag to flag
 */
void Script_CopyVal_b() {
  UBYTE value = script_variables[active_script_ctx.script_ptr_y];
  script_variables[active_script_ctx.script_ptr_x] = value;
}

/*
 * Command: IfValue
 * ----------------------------
 * Jump to new script pointer position if specified flag is true when compared using operator to
 * comparator.
 *
 *   arg0: Operator
 *   arg1: High 8 bits for new pointer
 *   arg2: Low 8 bits for new pointer
 */
void Script_IfValueCompare_b() {
  UBYTE match;
  UBYTE a = script_variables[active_script_ctx.script_ptr_x];
  UBYTE b = script_variables[active_script_ctx.script_ptr_y];

  switch (script_cmd_args[0]) {
    case OPERATOR_EQ:
      match = a == b;
      break;
    case OPERATOR_LT:
      match = a < b;
      break;
    case OPERATOR_LTE:
      match = a <= b;
      break;
    case OPERATOR_GT:
      match = a > b;
      break;
    case OPERATOR_GTE:
      match = a >= b;
      break;
    case OPERATOR_NE:
      match = a != b;
      break;
    default:
      match = FALSE;
  }

  if (match) {  // True path, jump to position specified by ptr
    active_script_ctx.script_ptr = active_script_ctx.script_start_ptr + (script_cmd_args[1] * 256) + script_cmd_args[2];
  }
}

/*
 * Command: LoadVectors
 * ----------------------------
 * Loads a vector pair
 *
 *   arg0: High 8 bits for first pointer
 *   arg1: Low 8 bits for first pointer
 *   arg2: High 8 bits for second pointer
 *   arg3: Low 8 bits for second pointer
 */
void Script_LoadVectors_b() {
  active_script_ctx.script_ptr_x = (script_cmd_args[0] * 256) + script_cmd_args[1];
  active_script_ctx.script_ptr_y = (script_cmd_args[2] * 256) + script_cmd_args[3];
}

/*
 * Command: ActorSetMoveSpeed
 * ----------------------------
 * Set active actor movement speed.
 *
 *   arg0: Movement speed to use
 */
void Script_ActorSetMoveSpeed_b() {
}

/*
 * Command: ActorSetAnimSpeed
 * ----------------------------
 * Set active actor animation speed.
 *
 *   arg0: Animation speed to use
 */
void Script_ActorSetAnimSpeed_b() {
}

/*
 * Command: TextSetAnimSpeed
 * ----------------------------
 * Set global text animation speed.
 *
 *   arg0: Animation speed to use
 *   arg1: Animation speed to use fading out
 */
void Script_TextSetAnimSpeed_b() {
  text_in_speed = script_cmd_args[0];
  text_out_speed = script_cmd_args[1];
  text_draw_speed = script_cmd_args[2];
  text_ff_joypad = script_cmd_args[3] ? (J_A | J_B) : 0;
}

/*
 * Command: ActorInvoke
 * ----------------------------
 * Invoke Actor script
 */
void Script_ActorInvoke_b() {

}

/*
 * Command: StackPush
 * ----------------------------
 * Push the current script pointer to the stack
 * Store script start ptr for if statements, script bank for long scripts.
 */
void Script_StackPush_b() {
}

/*
 * Command: StackPop
 * ----------------------------
 * Pop the script pointer from the stack
 * Retrieve script start ptr for if statements, script bank for long scripts.
 */
void Script_StackPop_b() {
  script_stack_ptr--;
  active_script_ctx.script_ptr_bank = script_bank_stack[script_stack_ptr];
  active_script_ctx.script_ptr = script_stack[script_stack_ptr];
  active_script_ctx.script_start_ptr = script_start_stack[script_stack_ptr];
}

/*
 * Command: ScenePushState
 * ----------------------------
 * Stores the state of the current scene
 */
void Script_ScenePushState_b() {
}

/*
 * Command: ScenePopState
 * ----------------------------
 * Restores the saved scene state
 *
 *   arg0: Fade speed
 */
void Script_ScenePopState_b() {
  UINT16 scene_next_index;

  if (scene_stack_ptr) {
    scene_stack_ptr--;

    scene_next_index = scene_stack[scene_stack_ptr].scene_index;

    return;
  }
}

/*
 * Command: SceneResetStack
 * ----------------------------
 * Clear all saved scene state
 */
void Script_SceneResetStack_b() {
  scene_stack_ptr = 0;
}

/*
 * Command: ScenePopAllState
 * ----------------------------
 * Restores the first saved scene state
 */
void Script_ScenePopAllState_b() {
  UINT16 scene_next_index;

  if (scene_stack_ptr) {
    scene_stack_ptr = 0;

    scene_next_index = scene_stack[scene_stack_ptr].scene_index;

    return;
  }
}

/*
 * Command: SetInputScript
 * ----------------------------
 * Attach script to button press
 */
void Script_SetInputScript_b() {
  UBYTE input, index;

  input = script_cmd_args[0];

  if (script_cmd_args[1]) {
    SET_BIT_MASK(input_script_persist, input);
  } else {
    UNSET_BIT_MASK(input_script_persist, input);
  }

  index = 0;
  while (!(input & 1) && input != 0) {
    index += 1;
    input = input >> 1;
  }

  input_script_ptrs[index].bank = script_cmd_args[2];
  input_script_ptrs[index].offset = (script_cmd_args[3] * 256) + script_cmd_args[4];
}

/*
 * Command: RemoveInputScript
 * ----------------------------
 * Remove script from button press
 */
void Script_RemoveInputScript_b() {
  UBYTE input, index;

  input = script_cmd_args[0];

  index = 0;
  for (index = 0; index != 8; ++index) {
    if (input & 1) {
      input_script_ptrs[index].bank = 0;
    }
    input = input >> 1;
  }
}

/*
 * Command: ActorSetFrame
 * ----------------------------
 * Set animation frame of current actor
 */
void Script_ActorSetFrame_b() {
}

/*
 * Command: ActorSetFrameToVal
 * ----------------------------
 * Set animation frame of current actor using variable
 */
void Script_ActorSetFrameToVal_b() {
}

/*
 * Command: ActorSetFlip
 * ----------------------------
 * Set flip state of current actor
 */
void Script_ActorSetFlip_b() { /* NOOP - This command has been removed */
}

/*
 * Command: TextMulti
 * ----------------------------
 * Set multi text box mode
 * mode - 0 (store text close speed and set to instant)
 *      - 1 (store open speed and set to instant)
 *      - 2 (restore close speed)
 *      - 3 (restore open and close speed)
 */
void Script_TextMulti_b() {
  UBYTE mode;
  mode = script_cmd_args[0];

  

  if (mode == 0) {
    tmp_text_out_speed = text_out_speed;
    text_out_speed = 0;
  } else if (mode == 1) {
    tmp_text_in_speed = text_in_speed;
    text_in_speed = 0;
  } else if (mode == 2) {
    text_out_speed = tmp_text_out_speed;
  } else if (mode == 3) {
    text_in_speed = tmp_text_in_speed;
    text_out_speed = tmp_text_out_speed;
  }

  
}

/*
 * Command: VariableAddFlags
 * ----------------------------
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_VariableAddFlags_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a | b;
}

/*
 * Command: VariableClearFlags
 * ----------------------------
 *   arg0: High 8 bits for flag index
 *   arg1: Low 8 bits for flag index
 *   arg2: Value
 */
void Script_VariableClearFlags_b() {
  UWORD ptr = (script_cmd_args[0] * 256) + script_cmd_args[1];
  UBYTE a = script_variables[ptr];
  UBYTE b = script_cmd_args[2];
  script_variables[ptr] = a & ~b;
}

/*
 * Command: SoundStartTone
 * ----------------------------
 */
void Script_SoundPlayTone_b() {
}

/*
 * Command: SoundStopTone
 * ----------------------------
 */
void Script_SoundStopTone_b() {
}

/*
 * Command: SoundPlayBeep
 * ----------------------------
 */
void Script_SoundPlayBeep_b() {
}

/*
 * Command: SoundPlayCrash
 * ----------------------------
 */
void Script_SoundPlayCrash_b() {
}

/*
 * Command: SetTimerScript
 * ----------------------------
 * Attach script to timer
 */
void Script_SetTimerScript_b() {
  timer_script_duration = script_cmd_args[0];
  timer_script_time = script_cmd_args[0];
  timer_script_ptr.bank = script_cmd_args[1];
  timer_script_ptr.offset = (script_cmd_args[2] * 256) + script_cmd_args[3];
}

/*
 * Command: ResetTimer
 * ----------------------------
 * Reset the countdown timer
 */
void Script_ResetTimer_b() {
  timer_script_time = timer_script_duration;
}

/*
 * Command: RemoveTimerScript
 * ----------------------------
 * Disable timer script
 */
void Script_RemoveTimerScript_b() {
  timer_script_duration = 0;
}

/*
 * Command: Text with Avatar
 * ----------------------------
 * Display a line of dialogue with a 16x16 avatar on the left
 *
 *   arg0: High 8 bits for string index
 *   arg1: Low 8 bits for string index
 *   arg2: Spritesheet to use as the dialogue avatar
 */
void Script_TextWithAvatar_b() {
  avatar_enabled = TRUE;
  UIShowText(script_cmd_args[0], (script_cmd_args[1] * 256) + script_cmd_args[2]);
  UIShowAvatar(script_cmd_args[3]);
  active_script_ctx.script_update_fn = ScriptUpdate_AwaitUIClosed;
}

void Script_LaunchProjectile_b() {
  
}

void Script_WeaponAttack_b() {

}

void Script_PalSetBackground_b() {
}

void Script_PalSetSprite_b() {
}

void Script_PalSetUI_b() {
}

void Script_ActorStopUpdate_b() {

}

void Script_ActorSetAnimate_b() {
}

void Script_IfColorSupported_b() {
}
