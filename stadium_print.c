#include <stdio.h>
#include <stdbool.h>
#include <hb/stadium.h>

static const char *
_hb_camera_follow_to_string(enum hb_camera_follow cf)
{
	switch (cf) {
	case HB_CAMERA_FOLLOW_BALL: return "ball";
	case HB_CAMERA_FOLLOW_PLAYER: return "player";
	default: return "unknown";
	}
}

static const char *
_hb_kick_off_reset_to_string(enum hb_kick_off_reset kor)
{
	switch (kor) {
	case HB_KICK_OFF_RESET_FULL: return "full";
	case HB_KICK_OFF_RESET_PARTIAL: return "partial";
	default: return "unknown";
	}
}

static const char *
_hb_bool_yes_no_to_string(bool b)
{
	if (b) return "yes";
	return "no";
}

static const char *
_hb_background_type_to_string(enum hb_background_type bt)
{
	switch (bt) {
	case HB_BACKGROUND_TYPE_NONE: return "none";
	case HB_BACKGROUND_TYPE_GRASS: return "grass";
	case HB_BACKGROUND_TYPE_HOCKEY: return "hockey";
	default: return "unknown";
	}
}

static const char *
_hb_team_to_string(enum hb_team team)
{
	switch (team) {
	case HB_TEAM_RED: return "red";
	case HB_TEAM_BLUE: return "blue";
	case HB_TEAM_SPECTATOR: return "spectator";
	default: return "unknown";
	}
}

static void
_hb_trait_print(struct hb_trait *t)
{
	if (t->has_curve) printf("Trait[%s].curve: %.2f\n", t->name, t->curve);
	if (t->has_damping) printf("Trait[%s].damping: %.2f\n", t->name, t->damping);
	if (t->has_inv_mass) printf("Trait[%s].invMass: %.2f\n", t->name, t->inv_mass);
	if (t->has_radius) printf("Trait[%s].radius: %.2f\n", t->name, t->radius);
	if (t->has_b_coef) printf("Trait[%s].bCoef: %.2f\n", t->name, t->b_coef);
	if (t->has_color) printf("Trait[%s].color: %08x\n", t->name, t->color);
	if (t->has_vis) printf("Trait[%s].vis: %s\n", t->name, _hb_bool_yes_no_to_string(t->vis));
	if (t->has_c_group) printf("Trait[%s].cGroup: %d\n", t->name, t->c_group);
	if (t->has_c_mask) printf("Trait[%s].cMask: %d\n", t->name, t->c_mask);
}

static void
_hb_vertex_print(int index, struct hb_vertex *v)
{
	printf("Vertex[%d].x: %.2f\n", index, v->x);
	printf("Vertex[%d].y: %.2f\n", index, v->y);
	printf("Vertex[%d].bCoef: %.2f\n", index, v->b_coef);
	printf("Vertex[%d].cGroup: %d\n", index, v->c_group);
	printf("Vertex[%d].cMask: %d\n", index, v->c_mask);
}

static void
_hb_segment_print(int index, struct hb_segment *s)
{
	printf("Segment[%d].v0: %d\n", index, s->v0);
	printf("Segment[%d].v1: %d\n", index, s->v1);
	printf("Segment[%d].bCoef: %.2f\n", index, s->b_coef);
	printf("Segment[%d].curve: %.2f\n", index, s->curve);
	printf("Segment[%d].bias: %.2f\n", index, s->bias);
	printf("Segment[%d].cGroup: %d\n", index, s->c_group);
	printf("Segment[%d].cMask: %d\n", index, s->c_mask);
	printf("Segment[%d].vis: %s\n", index, _hb_bool_yes_no_to_string(s->vis));
	printf("Segment[%d].color: %08x\n", index, s->color);
}

static void
_hb_goal_print(int index, struct hb_goal *g)
{
	printf("Goal[%d].p0: (%.2f, %.2f)\n", index, g->p0[0], g->p0[1]);
	printf("Goal[%d].p1: (%.2f, %.2f)\n", index, g->p1[0], g->p1[1]);
	printf("Goal[%d].team: %s\n", index, _hb_team_to_string(g->team));
}

static void
_hb_disc_print(int index, struct hb_disc *d)
{
	printf("Disc[%d].pos: (%.2f, %.2f)\n", index, d->pos[0], d->pos[1]);
	printf("Disc[%d].speed: (%.2f, %.2f)\n", index, d->speed[0], d->speed[1]);
	printf("Disc[%d].gravity: (%.2f, %.2f)\n", index, d->gravity[0], d->gravity[1]);
	printf("Disc[%d].radius: %.2f\n", index, d->radius);
	printf("Disc[%d].invMass: %.2f\n", index, d->inv_mass);
	printf("Disc[%d].damping: %.2f\n", index, d->damping);
	printf("Disc[%d].color: %08x\n", index, d->color);
	printf("Disc[%d].bCoef: %.2f\n", index, d->b_coef);
	printf("Disc[%d].cMask: %d\n", index, d->c_mask);
	printf("Disc[%d].cGroup: %d\n", index, d->c_group);
}

extern const char *
hb_stadium_print(struct hb_stadium *s)
{
	struct hb_trait **trait;
	struct hb_vertex **vertex;
	struct hb_segment **segment;
	struct hb_goal **goal;
	struct hb_disc **disc;

	printf("Name: %s\n", s->name);
	printf("CameraWidth: %.2f\n", s->camera_width);
	printf("CameraHeight: %.2f\n", s->camera_height);
	printf("MaxViewWidth: %.2f\n", s->max_view_width);
	printf("CameraFollow: %s\n", _hb_camera_follow_to_string(s->camera_follow));
	printf("SpawnDistance: %.2f\n", s->spawn_distance);
	printf("CanBeStored: %s\n", _hb_bool_yes_no_to_string(s->can_be_stored));
	printf("KickOffReset: %s\n", _hb_kick_off_reset_to_string(s->kick_off_reset));
	printf("Background.Type: %s\n", _hb_background_type_to_string(s->bg->type));
	printf("Background.Width: %.2f\n", s->bg->width);
	printf("Background.Height: %.2f\n", s->bg->height);
	printf("Background.KickOffRadius: %.2f\n", s->bg->kick_off_radius);
	printf("Background.CornerRadius: %.2f\n", s->bg->corner_radius);
	printf("Background.GoalLine: %.2f\n", s->bg->goal_line);
	printf("Background.Color: %08x\n", s->bg->color);

	for (trait = s->traits; *trait; ++trait) _hb_trait_print(*trait);
	for (vertex = s->vertexes; *vertex; ++vertex) _hb_vertex_print(vertex - s->vertexes, *vertex);
	for (segment = s->segments; *segment; ++segment) _hb_segment_print(segment - s->segments, *segment);
	for (goal = s->goals; *goal; ++goal) _hb_goal_print(goal - s->goals, *goal);
	for (disc = s->discs; *disc; ++disc) _hb_disc_print(disc - s->discs, *disc);
}

int
main(int argc, char **argv)
{
	struct hb_stadium *big;
	big = hb_stadium_from_file(argc > 1 ? argv[1] : "stadiums/big.hbs");
	if (!big) fprintf(stderr, "Bad stadium file!\n");
	else hb_stadium_print(big);
	return 0;
}
