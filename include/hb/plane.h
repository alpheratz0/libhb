#ifndef __LIBHB_PLANE_H__
#define __LIBHB_PLANE_H__

#include <hb/collision_flags.h>

struct hb_plane {
	float                         normal[2];
	float                              dist;
	float                            b_coef;
	enum hb_collision_flags          c_mask;
	enum hb_collision_flags         c_group;
};

#endif
