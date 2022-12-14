#ifndef __LIBHB_PLAYER_PHYSICS_H__
#define __LIBHB_PLAYER_PHYSICS_H__

#include <hb/collision_flags.h>

struct hb_player_physics {
	float                                 gravity[2];
	float                                     radius;
	float                                   inv_mass;
	float                                     b_coef;
	float                                    damping;
	enum hb_collision_flags                  c_group;
	float                               acceleration;
	float                       kicking_acceleration;
	float                            kicking_damping;
	float                              kick_strength;
	float                                   kickback;
};

#endif
