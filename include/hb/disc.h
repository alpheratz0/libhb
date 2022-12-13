#ifndef __LIBHB_DISC_H__
#define __LIBHB_DISC_H__

#include <stdint.h>
#include <hb/collision_flags.h>

struct hb_disc {
	float                            pos[2];
	float                          speed[2];
	float                        gravity[2];
	float                            radius;
	float                          inv_mass;
	float                           damping;
	uint32_t                          color;
	float                            b_coef;
	enum hb_collision_flags          c_mask;
	enum hb_collision_flags         c_group;
};

#endif
