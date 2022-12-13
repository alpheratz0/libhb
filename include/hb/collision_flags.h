#ifndef __LIBHB_COLLISION_FLAGS_H__
#define __LIBHB_COLLISION_FLAGS_H__

enum hb_collision_flags {
	HB_COLLISION_BALL =           1 << 0,
	HB_COLLISION_RED =            1 << 1,
	HB_COLLISION_BLUE =           1 << 2,
	HB_COLLISION_RED_KO =         1 << 3,
	HB_COLLISION_BLUE_KO =        1 << 4,
	HB_COLLISION_WALL =           1 << 5,
	HB_COLLISION_ALL =      (1 << 6) - 1,
	HB_COLLISION_KICK =           1 << 6,
	HB_COLLISION_SCORE =          1 << 7,
	HB_COLLISION_C0 =            1 << 28,
	HB_COLLISION_C1 =            1 << 29,
	HB_COLLISION_C2 =            1 << 30,
	HB_COLLISION_C3 =     (int)(1u << 31)
};

#endif
