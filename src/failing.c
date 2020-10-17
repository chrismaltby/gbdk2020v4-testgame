#include <gb/gb.h>
#include <stdio.h>

#define MAX_PROJECTILES 5
#define NO_ACTOR_COLLISON 0xFF
#define MAX_ACTIVE_ACTORS 11
#define MAX_ACTORS 31
#define MAX_SCRIPT_CONTEXTS 12

unsigned char test1;
unsigned char test2;

typedef struct _BankPtr
{
  unsigned char bank;
  unsigned int offset;
} BankPtr;

const BankPtr scene_bank_ptrs[] = {{0x06, 0x3C53}};

typedef struct _Pos {
  WORD x;
  WORD y;
} Pos;

typedef struct _Vector2D {
  BYTE x;
  BYTE y;
} Vector2D;

typedef enum { SPRITE_STATIC = 0, SPRITE_ACTOR, SPRITE_ACTOR_ANIMATED } SPRITE_TYPE;


typedef struct _PROJECTILE {
  Pos pos;
  Vector2D vel;
  Vector2D dir;
  UBYTE moving;
  UBYTE pin_actor;
  UBYTE sprite;
  UBYTE palette_index;
  SPRITE_TYPE sprite_type;
  UBYTE frame;
  UBYTE frames_len;
  UBYTE animate;
  UBYTE move_speed;
  UBYTE sprite_index;
  UBYTE time;
  UBYTE life_time;
  UBYTE col_group;
  UBYTE col_mask;
} Projectile;


typedef struct {
  Pos pos;  // 0
  UBYTE move_speed; // 4
  Vector2D dir; // 5
  UBYTE moving; // 7
  UBYTE sprite; // 8       // Offset into scene sprites image data
  UBYTE sprite_index; // 9  // Sprite pool index
  UBYTE palette_index; // 10
  Pos start_pos; // 11
  UBYTE frame; // 15
  UBYTE frames_len; // 16
  UBYTE animate; // 17
  UBYTE enabled; // 18
  UBYTE frame_offset; // 19
  UBYTE rerender; // 20
  UBYTE anim_speed; // 21
  UBYTE pinned; // 22
  UBYTE collisionsEnabled; // 23
  UBYTE collision_group; // 24
  SPRITE_TYPE sprite_type; // 25
  UBYTE hit_actor; // 26
  UBYTE script_control; // 27
  UWORD script_ptr; // 28
  BankPtr events_ptr; // 30
  BankPtr movement_ptr; // 33
  BankPtr hit_1_ptr; // 36
  BankPtr hit_2_ptr; // 39
  BankPtr hit_3_ptr; // 42  
  UBYTE movement_ctx; // 45
} Actor;

typedef struct {
  UINT16 actor_move_dest_x;
  UINT16 actor_move_dest_y;
  UBYTE* script_start_ptr;
  UBYTE* script_ptr;
  UWORD script_ptr_x;
  UWORD script_ptr_y;
  UBYTE script_ptr_bank;
  UBYTE wait_time;
  UBYTE script_await_next_frame;
  UBYTE script_actor;
  UBYTE owner;
  UBYTE actor_move_cols;
  UBYTE tmp_1;
  UBYTE tmp_2;
  UBYTE dummy24byte; // even struct faster index multiplication
} ScriptContext;

Projectile projectiles[MAX_PROJECTILES];
UBYTE actors_active[MAX_ACTIVE_ACTORS];
Actor actors[MAX_ACTORS];
ScriptContext script_ctxs[MAX_SCRIPT_CONTEXTS] = {{0}};

void TestFn2(unsigned char i)
{
  test2 = i;
}

void TestFn(unsigned int index)
{
  unsigned char bank;
  unsigned int data_ptr;

  bank = scene_bank_ptrs[index].bank;
  data_ptr = scene_bank_ptrs[index].offset;

  /* This section doesn't matter but the bug doesn't trigger without it */
  test1 = 1;
  TestFn2(bank);
  /**/

  if (data_ptr == 0x3C53)
  {
    printf("OK\n");
  }
  else
  {
    printf("FAIL\n");
  }
}

UBYTE ScriptStartBg2(BankPtr* events_ptr, UBYTE owner) {

  if (events_ptr->bank == 5) {
    printf("OK\n");
  }
  else
  {
    printf("FAIL\n");
  }  

  // UWORD new_ctx = 0;

  // // SeedRand();

  // // Run in background context
  // // new_ctx = ScriptCtxPoolNext();
  // new_ctx = 1;

  // if (new_ctx != 0) {
  //   // script_variables[TMP_VAR_1] = 0;
  //   // script_variables[TMP_VAR_2] = 0;

  //   script_ctxs[new_ctx].owner = 0;  // @wtf
  //   script_ctxs[new_ctx].script_ptr_bank = events_ptr->bank;
  //   script_ctxs[new_ctx].script_ptr = events_ptr->offset;
  //   // script_ctxs[new_ctx].script_update_fn = FALSE;
  //   // script_ctxs[new_ctx].script_start_ptr = script_ctxs[new_ctx].script_ptr;
  //   // script_ctxs[new_ctx].owner = owner;

  //   // ScriptRestoreCtx(new_ctx);

  //   return new_ctx;
  // }

  return 0;
}

void UpdateProjectiles_b() {
  UBYTE hit;
  UBYTE i;

  for (i = 0; i != 1; i++) {
      hit = actors_active[0];
      if (hit != 0xFF) {
        if (projectiles[i].col_group == 2) {
            ScriptStartBg2(&actors[hit].hit_1_ptr, hit);
        } else if (projectiles[i].col_group == 4) {
            ScriptStartBg2(&actors[hit].hit_2_ptr, hit);
        }
      }
  }
}



int main()
{
  LCDC_REG = 0x67;
  BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;
  WX_REG = MAXWNDPOSX;
  WY_REG = MAXWNDPOSY;

  projectiles[0].col_group = 2;
  actors_active[0] = 0;
  actors[0].hit_1_ptr.bank = 5;
  actors[0].hit_1_ptr.offset = 0xFC00;

  TestFn();

  return 0;
}
