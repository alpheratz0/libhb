#ifndef __LIBHB_STADIUM_H__
#define __LIBHB_STADIUM_H__

#include <stdbool.h>
#include <hb/background.h>
#include <hb/trait.h>
#include <hb/vertex.h>
#include <hb/segment.h>
#include <hb/goal.h>
#include <hb/disc.h>
#include <hb/plane.h>
#include <hb/joint.h>
#include <hb/point.h>
#include <hb/player_physics.h>

enum hb_camera_follow {
	HB_CAMERA_FOLLOW_PLAYER,
	HB_CAMERA_FOLLOW_BALL
};

enum hb_kick_off_reset {
	HB_KICK_OFF_RESET_PARTIAL,
	HB_KICK_OFF_RESET_FULL
};

struct hb_stadium {
	char                                  *name;
	double                                width;
	double                               height;
	double                         camera_width;
	double                        camera_height;
	double                       max_view_width;
	enum hb_camera_follow         camera_follow;
	double                       spawn_distance;
	bool                          can_be_stored;
	enum hb_kick_off_reset       kick_off_reset;
	struct hb_background                    *bg;
	struct hb_trait                    **traits;
	struct hb_vertex                 **vertexes;
	struct hb_segment                **segments;
	struct hb_goal                      **goals;
	struct hb_disc                      **discs;
	struct hb_plane                    **planes;
	struct hb_joint                    **joints;
	struct hb_disc                *ball_physics;
	struct hb_point          **red_spawn_points;
	struct hb_point         **blue_spawn_points;
	struct hb_player_physics    *player_physics;
};

extern struct hb_stadium *
hb_stadium_parse(const char *in);

extern struct hb_stadium *
hb_stadium_from_file(const char *file);

extern char *
hb_stadium_to_json(const struct hb_stadium *s);

extern void
hb_stadium_print(const struct hb_stadium *s);

extern void
hb_stadium_free(struct hb_stadium *s);

#define hb_stadium_traits_foreach(s,t) \
	if (s->traits) \
		for (struct hb_trait *t, **v = s->traits; (t = *v); ++v) \

#define hb_stadium_vertexes_foreach(s,t) \
	if (s->vertexes) \
		for (struct hb_vertex *t, **v = s->vertexes; (t = *v); ++v) \

#define hb_stadium_segments_foreach(s,t) \
	if (s->segments) \
		for (struct hb_segment *t, **v = s->segments; (t = *v); ++v) \

#define hb_stadium_goals_foreach(s,t) \
	if (s->goals) \
		for (struct hb_goal *t, **v = s->goals; (t = *v); ++v) \

#define hb_stadium_discs_foreach(s,t) \
	if (s->discs) \
		for (struct hb_disc *t, **v = s->discs; (t = *v); ++v) \

#define hb_stadium_planes_foreach(s,t) \
	if (s->planes) \
		for (struct hb_plane *t, **v = s->planes; (t = *v); ++v)\

#define hb_stadium_joints_foreach(s,t) \
	if (s->joints) \
		for (struct hb_joint *t, **v = s->joints; (t = *v); ++v) \

#define hb_stadium_red_spawn_points_foreach(s,t) \
	if (s->red_spawn_points) \
		for (struct hb_point *t, **v = s->red_spawn_points; (t = *v); ++v) \

#define hb_stadium_blue_spawn_points_foreach(s,t) \
	if (s->blue_spawn_points) \
		for (struct hb_point *t, **v = s->blue_spawn_points; (t = *v); ++v) \

#endif
