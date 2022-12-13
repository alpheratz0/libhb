#ifndef __LIBHB_JOINT_H__
#define __LIBHB_JOINT_H__

#include <stdbool.h>
#include <stdint.h>

enum hb_joint_length_kind {
	HB_JOINT_LENGTH_FIXED,
	HB_JOINT_LENGTH_RANGE,
	HB_JOINT_LENGTH_AUTO
};

union hb_joint_length_value {
	float                                 f;
	float                          range[2];
};

struct hb_joint_length {
	enum hb_joint_length_kind          kind;
	union hb_joint_length_value         val;
};

struct hb_joint_strength {
	bool                           is_rigid;
	float                               val;
};

struct hb_joint {
	int                                  d0;
	int                                  d1;
	struct hb_joint_length           length;
	struct hb_joint_strength       strength;
	uint32_t                          color;
};

#endif
