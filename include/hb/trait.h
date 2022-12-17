#ifndef __LIBHB_TRAIT_H__
#define __LIBHB_TRAIT_H__

#include <stdbool.h>
#include <hb/collision_flags.h>

struct hb_trait {
	char                              *name;
	bool                          has_curve;
	double                            curve;
	bool                        has_damping;
	double                          damping;
	bool                       has_inv_mass;
	double                         inv_mass;
	bool                         has_radius;
	double                           radius;
	bool                         has_b_coef;
	double                           b_coef;
	bool                          has_color;
	uint32_t                          color;
	bool                            has_vis;
	bool                                vis;
	bool                        has_c_group;
	enum hb_collision_flags         c_group;
	bool                         has_c_mask;
	enum hb_collision_flags          c_mask;
};

#endif
