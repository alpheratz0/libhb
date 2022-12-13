#ifndef __LIBHB_BACKGROUND_H__
#define __LIBHB_BACKGROUND_H__

#include <stdint.h>

enum hb_background_type {
	HB_BACKGROUND_TYPE_NONE,
	HB_BACKGROUND_TYPE_GRASS,
	HB_BACKGROUND_TYPE_HOCKEY
};

struct hb_background {
	enum hb_background_type            type;
	float                             width;
	float                            height;
	float                   kick_off_radius;
	float                     corner_radius;
	float                         goal_line;
	uint32_t                          color;
};

#endif
