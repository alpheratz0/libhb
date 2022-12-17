#ifndef __LIBHB_PLAYER_PHYSICS_H__
#define __LIBHB_PLAYER_PHYSICS_H__

#include <hb/collision_flags.h>

struct hb_player_physics {
	double                                gravity[2];
	double                                    radius;
	double                                  inv_mass;
	double                                    b_coef;
	double                                   damping;
	enum hb_collision_flags                  c_group;
	double                              acceleration;
	double                      kicking_acceleration;
	double                           kicking_damping;
	double                             kick_strength;
	double                                  kickback;
};

#endif
