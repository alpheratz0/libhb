#ifndef __LIBHB_DISC_H__
#define __LIBHB_DISC_H__

#include <stdint.h>
#include <hb/collision_flags.h>

struct hb_disc {
	double                           pos[2];
	double                         speed[2];
	double                       gravity[2];
	double                           radius;
	double                         inv_mass;
	double                          damping;
	uint32_t                          color;
	double                           b_coef;
	enum hb_collision_flags          c_mask;
	enum hb_collision_flags         c_group;
};

#endif
