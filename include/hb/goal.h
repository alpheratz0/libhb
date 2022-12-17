#ifndef __LIBHB_GOAL_H__
#define __LIBHB_GOAL_H__

#include <hb/team.h>

struct hb_goal {
	double                            p0[2];
	double                            p1[2];
	enum hb_team                       team;
};

#endif
