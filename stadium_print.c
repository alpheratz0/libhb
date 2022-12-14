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
	printf("Trait.%-15s: ", t->name);
	if (t->has_curve) printf("Curve: %.2f, ", t->curve);
	if (t->has_damping) printf("Damping: %.2f, ", t->damping);
	if (t->has_inv_mass) printf("InvMass: %.2f, ", t->inv_mass);
	if (t->has_radius) printf("Radius: %.2f, ", t->radius);
	if (t->has_b_coef) printf("BCoef: %.2f, ", t->b_coef);
	if (t->has_color) printf("Color: %08x, ", t->color);
	if (t->has_vis) printf("Vis: %s, ", _hb_bool_yes_no_to_string(t->vis));
	if (t->has_c_group) printf("CGroup: %d, ", t->c_group);
	if (t->has_c_mask) printf("CMask: %d", t->c_mask);
	printf("\n");
}

static void
_hb_vertex_print(int index, struct hb_vertex *v)
{
	printf("Vertex.%-3d           : X: %8.2f, Y: %8.2f, BCoef: %4.2f, CGroup: %3d, CMask: %3d\n", index,
			v->x, v->y, v->b_coef, v->c_group, v->c_mask);
}

static void
_hb_segment_print(int index, struct hb_segment *s)
{
	printf("Segment.%-3d          : V0: %3d, V1: %3d, BCoef: %4.2f, Curve: %8.2f, "
			"Bias: %3.2f, CGroup: %3d, CMask: %3d, Vis: %3s, Color: %08x\n", index,
			s->v0, s->v1, s->b_coef, s->curve, s->bias, s->c_group, s->c_mask,
			_hb_bool_yes_no_to_string(s->vis), s->color);
}

static void
_hb_goal_print(int index, struct hb_goal *g)
{
	printf("Goal.%-3d             : P0: [%.2f, %.2f], P1: [%.2f, %.2f], Team: %s\n", index,
			g->p0[0], g->p0[1], g->p1[0], g->p1[1], _hb_team_to_string(g->team));
}

static void
_hb_disc_print(int index, struct hb_disc *d)
{
	printf("Disc.%-3d             : P: [%.1f, %.1f], S: [%.1f, %.1f], G: [%.1f, %.1f], Radius: %.1f, "
		   "InvMass: %.2f, Damping: %.2f, Color: %08x, BCoef: %.2f, CMask: %d, CGroup: %d\n", index,
			d->pos[0], d->pos[1], d->speed[0], d->speed[1], d->gravity[0], d->gravity[1],
			d->radius, d->inv_mass, d->damping, d->color, d->b_coef, d->c_mask, d->c_group);
}

static void
_hb_plane_print(int index, struct hb_plane *p)
{
	printf("Plane.%-3d            : Normal: [%4.1f, %4.1f], Dist: %4.2f, BCoef: %4.2f, CMask: %3d, CGroup: %3d\n", index,
			p->normal[0], p->normal[1], p->dist, p->b_coef, p->c_mask, p->c_group);
}

static void
_hb_joint_print(int index, struct hb_joint *j)
{
	printf("Joint.%-3d            : D0: %3d, D1: %3d, Color: %08x, ", index,
			j->d0, j->d1, j->color);
	if (j->length.kind == HB_JOINT_LENGTH_RANGE) {
		printf("Length: [%5.2f, %5.2f], ", j->length.val.range[0], j->length.val.range[1]);
	} else if (j->length.kind == HB_JOINT_LENGTH_FIXED) {
		printf("Length: %5.2f, ", j->length.val.f);
	} else {
		printf("Length: auto, ");
	}
	if (j->strength.is_rigid) printf("Strength: rigid\n");
	else printf("Strength: %5.2f\n", j->strength.val);
}

extern void
hb_stadium_print(struct hb_stadium *s)
{
	struct hb_trait **trait;
	struct hb_vertex **vertex;
	struct hb_segment **segment;
	struct hb_goal **goal;
	struct hb_disc **disc;
	struct hb_plane **plane;
	struct hb_joint **joint;
	struct hb_point **point;

	printf("Name                 : %s\n", s->name);
	printf("CameraWidth          : %.2f\n", s->camera_width);
	printf("CameraHeight         : %.2f\n", s->camera_height);
	printf("MaxViewWidth         : %.2f\n", s->max_view_width);
	printf("CameraFollow         : %s\n", _hb_camera_follow_to_string(s->camera_follow));
	printf("SpawnDistance        : %.2f\n", s->spawn_distance);
	printf("CanBeStored          : %s\n", _hb_bool_yes_no_to_string(s->can_be_stored));
	printf("KickOffReset         : %s\n", _hb_kick_off_reset_to_string(s->kick_off_reset));
	printf("Background           : Type: %s, Width: %.2f, Height: %.2f, KickOffRadius: %.2f\n"
		   "                       CornerRadius: %.2f, GoalLine: %.2f, Color: %08x\n",
			_hb_background_type_to_string(s->bg->type),
			s->bg->width, s->bg->height,
			s->bg->kick_off_radius, s->bg->corner_radius,
			s->bg->goal_line, s->bg->color);
	for (trait = s->traits; *trait; ++trait) _hb_trait_print(*trait);
	for (vertex = s->vertexes; *vertex; ++vertex) _hb_vertex_print(vertex - s->vertexes, *vertex);
	for (segment = s->segments; *segment; ++segment) _hb_segment_print(segment - s->segments, *segment);
	for (goal = s->goals; *goal; ++goal) _hb_goal_print(goal - s->goals, *goal);
	for (disc = s->discs; *disc; ++disc) _hb_disc_print(disc - s->discs, *disc);
	for (plane = s->planes; *plane; ++plane) _hb_plane_print(plane - s->planes, *plane);
	for (joint = s->joints; *joint; ++joint) _hb_joint_print(joint - s->joints, *joint);
	printf("RedSpawnPoints       : [");
	for (point = s->red_spawn_points; *point; ++point)
		printf("[%3.2f, %3.2f] ", (*point)->x, (*point)->y);
	printf("]\n");
	printf("BlueSpawnPoints      : [");
	for (point = s->blue_spawn_points; *point; ++point)
		printf("[%3.2f, %3.2f] ", (*point)->x, (*point)->y);
	printf("]\n");
}

int
main(int argc, char **argv)
{
	struct hb_stadium *big;

	if (argc < 2) return 1;
	big = hb_stadium_from_file(argv[1]);

	if (!big) return 1;

	hb_stadium_print(big);
	hb_stadium_free(big);

	return 0;
}
