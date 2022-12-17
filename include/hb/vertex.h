#ifndef __LIBHB_VERTEX_H__
#define __LIBHB_VERTEX_H__

#include <hb/collision_flags.h>

struct hb_vertex {
	double                                x;
	double                                y;
	double                           b_coef;
	enum hb_collision_flags         c_group;
	enum hb_collision_flags          c_mask;
};

#endif
