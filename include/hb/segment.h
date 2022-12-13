#ifndef __LIBHB_SEGMENT_H__
#define __LIBHB_SEGMENT_H__

#include <stdint.h>
#include <stdbool.h>
#include <hb/collision_flags.h>

struct hb_segment {
	int                                  v0;
	int                                  v1;
	float                            b_coef;
	float                             curve;
	float                              bias;
	enum hb_collision_flags         c_group;
	enum hb_collision_flags          c_mask;
	bool                                vis;
	uint32_t                          color;
};

#endif
