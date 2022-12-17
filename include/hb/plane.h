#ifndef __LIBHB_PLANE_H__
#define __LIBHB_PLANE_H__

#include <hb/collision_flags.h>

struct hb_plane {
	double                        normal[2];
	double                             dist;
	double                           b_coef;
	enum hb_collision_flags          c_mask;
	enum hb_collision_flags         c_group;
};

#endif
