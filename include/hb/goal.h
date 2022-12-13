#ifndef __LIBHB_GOAL_H__
#define __LIBHB_GOAL_H__

#include <hb/team.h>

struct hb_goal {
	float                             p0[2];
	float                             p1[2];
	enum hb_team                       team;
};

#endif
