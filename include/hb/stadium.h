#ifndef __LIBHB_STADIUM_H__
#define __LIBHB_STADIUM_H__

#include <stdbool.h>
#include <stdint.h>

enum hb_camera_follow {
	HB_CAMERA_FOLLOW_PLAYER,
	HB_CAMERA_FOLLOW_BALL
};

enum hb_kick_off_reset {
	HB_KICK_OFF_RESET_PARTIAL,
	HB_KICK_OFF_RESET_FULL
};

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

enum hb_collision_flags {
	HB_COLLISION_BALL =           1 << 0,
	HB_COLLISION_RED =            1 << 1,
	HB_COLLISION_BLUE =           1 << 2,
	HB_COLLISION_RED_KO =         1 << 3,
	HB_COLLISION_BLUE_KO =        1 << 4,
	HB_COLLISION_WALL =           1 << 5,
	HB_COLLISION_ALL =      (1 << 6) - 1,
	HB_COLLISION_KICK =           1 << 6,
	HB_COLLISION_SCORE =          1 << 7,
	HB_COLLISION_C0 =            1 << 28,
	HB_COLLISION_C1 =            1 << 29,
	HB_COLLISION_C2 =            1 << 30,
	HB_COLLISION_C3 =            1 << 31
};

struct hb_trait {
	char                              *name;
	bool                          has_curve;
	float                             curve;
	bool                        has_damping;
	float                           damping;
	bool                       has_inv_mass;
	float                          inv_mass;
	bool                         has_radius;
	float                            radius;
	bool                         has_b_coef;
	float                            b_coef;
	bool                          has_color;
	uint32_t                          color;
	bool                            has_vis;
	bool                                vis;
	bool                        has_c_group;
	enum hb_collision_flags         c_group;
	bool                         has_c_mask;
	enum hb_collision_flags          c_mask;
};

struct hb_vertex {
	float                                 x;
	float                                 y;
	float                            b_coef;
	enum hb_collision_flags         c_group;
	enum hb_collision_flags          c_mask;
};

struct hb_stadium {
	char                              *name;
	float                      camera_width;
	float                     camera_height;
	float                    max_view_width;
	enum hb_camera_follow     camera_follow;
	float                    spawn_distance;
	bool                      can_be_stored;
	enum hb_kick_off_reset   kick_off_reset;
	struct hb_background                *bg;
	struct hb_trait                **traits;
	struct hb_vertex             **vertexes;
};

extern struct hb_stadium *
hb_stadium_parse(const char *in);

extern struct hb_stadium *
hb_stadium_from_file(const char *file);

extern void
hb_stadium_free(struct hb_stadium *s);

extern const char *
hb_stadium_print(struct hb_stadium *s);

#endif
