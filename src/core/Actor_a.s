.include "asm.macros.s"

    GBC = 1

    .POS_Y_OFFSET = 0x02
    .DIR_X_OFFSET = 0x05
    .DIR_Y_OFFSET = 0x06
    .MOVING_OFFSET = 0x07
    .PINNED_OFFSET = 0x16
    .SPRITE_OFFSET = 0x08
    .SPRITE_INDEX_OFFSET = 0x09
    .PALETTE_INDEX_OFFSET = 0x0A
    .FRAME_OFFSET = 0x0F
    .FRAMES_LEN_OFFSET = 0x10
    .ANIMATE_OFFSET = 0x11
    .ENABLED_OFFSET = 0x12
    .RERENDER_OFFSET = 0x14
    .ANIM_SPEED_OFFSET = 0x15
    .SPRITE_TYPE_OFFSET = 0x19

    .SCRIPT_ACTOR_OFFSET = 0x11

    .SPRITE_STATIC = 0x0
    .SPRITE_ACTOR = 0x1
    .SPRITE_ACTOR_ANIMATED = 0x2

    .FLIP_BIT = 0x5

    .SCREENWIDTH_PLUS_64 = 224
    .SCREENHEIGHT_PLUS_64 = 208

.set_spr_prop:                                  ;; c - sprite no, e - value, preserves bc, spoils hl
        push bc
        ld hl, #_shadow_OAM+3 
        sla c
        sla c
        ld b, #0
        add hl, bc
        ld a, e
        ld (hl), a
        pop bc
        ret

.set_spr_tile:                                  ;; c - sprite no, b - value, preserves bc, spoils hl
        push bc
        ld hl, #_shadow_OAM+2 
        sla c
        sla c
        ld b, #0
        add hl, bc
        ld a, b
        ld (hl), a
        pop bc
        ret
.mv_spr:                                        ;; a - spr no, b - Y, c - X, preserves a, bc, spoils hl 
        push af
        ld hl, #_shadow_OAM
        add a
        add a 
        _add_a h, l
        ld a, b
        ld (hl+), a
        ld a, c
        ld (hl), a
        pop af
        ret

_UpdateActors::

    ; Reset delete counter
        xor a
        ret