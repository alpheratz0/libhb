#include <hb/stadium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jv.h>

static const float      HB_F_ZERO         = 0.0f;
static const bool       HB_B_TRUE         = true;
static const uint32_t   HB_COLOR_DEFAULT  = 0xff718c5a;

static long
_get_file_size(FILE *fp)
{
	long current_pos;
	long file_size;

	current_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, current_pos, SEEK_SET);

	return file_size;
}

static int
_hb_jv_parse_string(jv from, char **to, const char *fallback)
{
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		*to = strdup(jv_string_value(from));
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		*to = strdup(fallback);
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_number(jv from, float *to, const float *fallback)
{
	switch (jv_get_kind(from)) {
	case JV_KIND_NUMBER:
		*to = jv_number_value(from);
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		*to = *fallback;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_boolean(jv from, bool *to, const bool *fallback)
{
	switch (jv_get_kind(from)) {
	case JV_KIND_FALSE:
		*to = false;
		return 0;
	case JV_KIND_TRUE:
		*to = true;
		return 0;
	case JV_KIND_INVALID:
		if (NULL == fallback)
			return -1;
		*to = *fallback;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_color(jv from, uint32_t *color, const uint32_t *fallback)
{
	jv_kind kind;
	const char *color_str;
	char *color_str_end;

	kind = jv_get_kind(from);

	if (kind == JV_KIND_INVALID) {
		if (NULL == fallback)
			return -1;
		*color = *fallback;
		return 0;
	}

	if (kind == JV_KIND_STRING) {
		color_str = jv_string_value(from);
		if (!strcmp(color_str, "transparent")) *color = 0x00000000;
		else {
			*color = strtol(color_str, &color_str_end, 16);
			if (color_str_end - color_str != 6 ||
					color_str_end[1] != '\0')
				return -1;
		}
		return 0;
	}

	if (kind == JV_KIND_ARRAY) {
		if (jv_array_length(from) != 3)
			return -1;
		*color = 0xff << 24;
		jv_array_foreach(from, index, value) {
			if (jv_get_kind(value) != JV_KIND_NUMBER)
				return -1;
			*color |= ((int)(jv_number_value(value)) & 0xff) << (8*(2-index));
		}
		return 0;
	}

	return -1;
}

static int
_hb_jv_parse_bg(jv from, struct hb_background *bg)
{
	jv_kind kind;

	kind = jv_get_kind(from);

	bg->type = HB_BACKGROUND_TYPE_NONE;
	bg->width = 0.0f;
	bg->height = 0.0f;
	bg->kick_off_radius = 0.0f;
	bg->corner_radius = 0.0f;
	bg->goal_line = 0.0f;
	bg->color = 0xff718c5a;

	if (kind == JV_KIND_INVALID)
		return 0;
	if (kind != JV_KIND_OBJECT)
		return -1;

	/////////////type
	{
		jv type;
		char *type_str;
		type = jv_object_get(jv_copy(from), jv_string("type"));
		if (_hb_jv_parse_string(type, &type_str, "none") < 0)
			return -1;
		if (!strcmp(type_str, "none")) bg->type = HB_BACKGROUND_TYPE_NONE;
		else if (!strcmp(type_str, "grass")) bg->type = HB_BACKGROUND_TYPE_GRASS;
		else if (!strcmp(type_str, "hockey")) bg->type = HB_BACKGROUND_TYPE_HOCKEY;
		else { free(type_str); return -1; }
		free(type_str);
	}

	/////////////width & height
	{
		jv width, height;
		width = jv_object_get(jv_copy(from), jv_string("width"));
		height = jv_object_get(jv_copy(from), jv_string("height"));
		if (_hb_jv_parse_number(width, &bg->width, &HB_F_ZERO) < 0 ||
				_hb_jv_parse_number(height, &bg->height, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////kickOffRadius
	{
		jv kick_off_radius;
		kick_off_radius = jv_object_get(jv_copy(from), jv_string("kickOffRadius"));
		if (_hb_jv_parse_number(kick_off_radius, &bg->kick_off_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////cornerRadius
	{
		jv corner_radius;
		corner_radius = jv_object_get(jv_copy(from), jv_string("cornerRadius"));
		if (_hb_jv_parse_number(corner_radius, &bg->corner_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////goalLine
	{
		jv goal_line;
		goal_line = jv_object_get(jv_copy(from), jv_string("goalLine"));
		if (_hb_jv_parse_number(goal_line, &bg->goal_line, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color(color, &bg->color, &HB_COLOR_DEFAULT) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_collision_flag(jv from, enum hb_collision_flags *flag)
{
	jv_kind kind;
	const char *str;
	kind = jv_get_kind(from);
	if (kind != JV_KIND_STRING) return -1;
	str = jv_string_value(from);
	if (!strcmp(str, "ball")) *flag = HB_COLLISION_BALL;
	else if (!strcmp(str, "red")) *flag = HB_COLLISION_RED;
	else if (!strcmp(str, "blue")) *flag = HB_COLLISION_BLUE;
	else if (!strcmp(str, "redKO")) *flag = HB_COLLISION_RED_KO;
	else if (!strcmp(str, "blueKO")) *flag = HB_COLLISION_BLUE_KO;
	else if (!strcmp(str, "wall")) *flag = HB_COLLISION_WALL;
	else if (!strcmp(str, "all")) *flag = HB_COLLISION_ALL;
	else if (!strcmp(str, "kick")) *flag = HB_COLLISION_KICK;
	else if (!strcmp(str, "score")) *flag = HB_COLLISION_SCORE;
	else if (!strcmp(str, "c0")) *flag = HB_COLLISION_C0;
	else if (!strcmp(str, "c1")) *flag = HB_COLLISION_C1;
	else if (!strcmp(str, "c2")) *flag = HB_COLLISION_C2;
	else if (!strcmp(str, "c3")) *flag = HB_COLLISION_C3;
	else return -1;
	return 0;
}

static int
_hb_jv_parse_collision_flags(jv from, enum hb_collision_flags *cf,
		const enum hb_collision_flags *fallback)
{
	jv_kind kind;
	enum hb_collision_flags flag;
	kind = jv_get_kind(from);
	if (kind == JV_KIND_INVALID) {
		if (NULL == fallback)
			return -1;
		*cf = *fallback;
		return 0;
	}
	if (kind != JV_KIND_ARRAY)
		return -1;
	jv_array_foreach(from, index, value) {
		if (_hb_jv_parse_collision_flag(value, &flag) < 0)
			return -1;
		*cf |= flag;
	}
	return 0;
}

static int
_hb_jv_parse_trait(jv from, struct hb_trait *trait)
{
	jv_kind kind;

	kind = jv_get_kind(from);

	if (kind != JV_KIND_OBJECT)
		return -1;

	/////////////curve
	{
		jv curve;
		curve = jv_object_get(jv_copy(from), jv_string("curve"));
		if (jv_get_kind(curve) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number(curve, &trait->curve, NULL) < 0)
				return -1;
			trait->has_curve = true;
		}
	}

	/////////////damping
	{
		jv damping;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (jv_get_kind(damping) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number(damping, &trait->damping, NULL) < 0)
				return -1;
			trait->has_damping = true;
		}
	}

	/////////////invMass
	{
		jv inv_mass;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (jv_get_kind(inv_mass) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number(inv_mass, &trait->inv_mass, NULL) < 0)
				return -1;
			trait->has_inv_mass = true;
		}
	}

	/////////////radius
	{
		jv radius;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (jv_get_kind(radius) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number(radius, &trait->radius, NULL) < 0)
				return -1;
			trait->has_radius = true;
		}
	}

	/////////////bCoef
	{
		jv b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (jv_get_kind(b_coef) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number(b_coef, &trait->b_coef, NULL) < 0)
				return -1;
			trait->has_b_coef = true;
		}
	}

	/////////////color
	{
		jv color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (jv_get_kind(color) != JV_KIND_INVALID) {
			if (_hb_jv_parse_color(color, &trait->color, NULL) < 0)
				return -1;
			trait->has_color = true;
		}
	}

	/////////////vis
	{
		jv vis;
		vis = jv_object_get(jv_copy(from), jv_string("vis"));
		trait->has_vis = jv_get_kind(vis) == JV_KIND_TRUE ||
			jv_get_kind(vis) == JV_KIND_FALSE;
		trait->vis = jv_get_kind(vis) == JV_KIND_TRUE;
	}

	/////////////cGroup
	{
		jv c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (jv_get_kind(c_group) != JV_KIND_INVALID) {
			if (_hb_jv_parse_collision_flags(c_group, &trait->c_group, NULL) < 0)
				return -1;
			trait->has_c_group = true;
		}
	}

	/////////////cMask
	{
		jv c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (jv_get_kind(c_mask) != JV_KIND_INVALID) {
			if (_hb_jv_parse_collision_flags(c_mask, &trait->c_mask, NULL) < 0)
				return -1;
			trait->has_c_mask = true;
		}
	}

	return 0;
}

static int
_hb_get_trait(struct hb_trait **traits, struct hb_trait **trait, const char *name)
{
	struct hb_trait **cur;
	for (cur = traits; *cur; ++cur) {
		if (!strcmp((*cur)->name, name)) {
			*trait = *cur;
			return 0;
		}
	}

	return -1;
}

static int
_hb_jv_parse_vertex(jv from, struct hb_vertex *to, struct hb_trait **traits)
{
	struct hb_trait *vertex_trait;

	/////////////x
	{
		jv x;
		x = jv_object_get(jv_copy(from), jv_string("x"));
		if (_hb_jv_parse_number(x, &to->x, NULL) < 0)
			return -1;
	}

	/////////////y
	{
		jv y;
		y = jv_object_get(jv_copy(from), jv_string("y"));
		if (_hb_jv_parse_number(y, &to->y, NULL) < 0)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		char *trait_name;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_parse_string(trait, &trait_name, "__no_trait__") < 0)
			return -1;
		if (!strcmp(trait_name, "__no_trait__"))
			vertex_trait = NULL;
		else if (_hb_get_trait(traits, &vertex_trait, trait_name) < 0) {
			free(trait_name);
			return -1;
		}
		free(trait_name);
	}

	/////////////bCoef
	{
		jv b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (jv_get_kind(b_coef) == JV_KIND_INVALID) {
			if (NULL == vertex_trait || !vertex_trait->has_b_coef)
				return -1;
			to->b_coef = vertex_trait->b_coef;
		} else {
			if (_hb_jv_parse_number(b_coef, &to->b_coef, NULL) < 0)
				return -1;
		}
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = 0;
		if (NULL != vertex_trait && vertex_trait->has_c_group)
			fallback_c_group = vertex_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags(c_group, &to->c_group, &fallback_c_group) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = 0;
		if (NULL != vertex_trait && vertex_trait->has_c_mask)
			fallback_c_mask = vertex_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags(c_mask, &to->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	return 0;
}

extern struct hb_stadium *
hb_stadium_parse(const char *in)
{
	struct hb_stadium *s;
	jv root;

	/////////////setup
	s = calloc(1, sizeof(struct hb_stadium));
	root = jv_parse(in);

	if (jv_get_kind(root) == JV_KIND_INVALID)
		goto err;

	/////////////name
	{
		jv name;
		name = jv_object_get(jv_copy(root), jv_string("name"));
		if (_hb_jv_parse_string(name, &s->name, NULL) < 0)
			goto err;
	}

	/////////////cameraWidth & cameraHeight
	{
		jv cam_width, cam_height;
		cam_width = jv_object_get(jv_copy(root), jv_string("cameraWidth"));
		cam_height = jv_object_get(jv_copy(root), jv_string("cameraHeight"));
		if (_hb_jv_parse_number(cam_width, &s->camera_width, &HB_F_ZERO) < 0
				&& _hb_jv_parse_number(cam_height, &s->camera_height, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////maxViewWidth
	{
		jv max_view_width;
		max_view_width = jv_object_get(jv_copy(root), jv_string("maxViewWidth"));
		if (_hb_jv_parse_number(max_view_width, &s->max_view_width, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////cameraFollow
	{
		jv camera_follow;
		char *camera_follow_str;
		camera_follow = jv_object_get(jv_copy(root), jv_string("cameraFollow"));
		if (_hb_jv_parse_string(camera_follow, &camera_follow_str, "ball") < 0)
			goto err;
		if (!strcmp(camera_follow_str, "ball")) {
			s->camera_follow = HB_CAMERA_FOLLOW_BALL;
		} else if (!strcmp(camera_follow_str, "player")) {
			s->camera_follow = HB_CAMERA_FOLLOW_PLAYER;
		} else {
			goto err;
		}
		free(camera_follow_str);
	}

	/////////////spawnDistance
	{
		jv spawn_distance;
		spawn_distance = jv_object_get(jv_copy(root), jv_string("spawnDistance"));
		if (_hb_jv_parse_number(spawn_distance, &s->spawn_distance, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////canBeStored
	{
		jv can_be_stored;
		can_be_stored = jv_object_get(jv_copy(root), jv_string("canBeStored"));
		if (_hb_jv_parse_boolean(can_be_stored, &s->can_be_stored, &HB_B_TRUE) < 0)
			goto err;
	}

	/////////////kickOffReset
	{
		jv kick_off_reset;
		char *kick_off_reset_str;
		kick_off_reset = jv_object_get(jv_copy(root), jv_string("kickOffReset"));
		if (_hb_jv_parse_string(kick_off_reset, &kick_off_reset_str, "partial") < 0)
			goto err;
		if (!strcmp(kick_off_reset_str, "partial")) {
			s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
		} else if (!strcmp(kick_off_reset_str, "full")) {
			s->kick_off_reset = HB_KICK_OFF_RESET_FULL;
		} else {
			goto err;
		}
		free(kick_off_reset_str);
	}

	/////////////bg
	{
		jv bg;
		bg = jv_object_get(jv_copy(root), jv_string("bg"));
		s->bg = malloc(sizeof(struct hb_background));
		if (_hb_jv_parse_bg(bg, s->bg) < 0)
			goto err;
	}

	/////////////traits
	{
		jv traits;
		jv_kind traits_kind;
		int traits_len;
		int trait_index;

		traits = jv_object_get(jv_copy(root), jv_string("traits"));
		traits_kind = jv_get_kind(traits);

		if (traits_kind == JV_KIND_OBJECT) {
			trait_index = 0;
			traits_len = jv_object_length(jv_copy(traits));
			s->traits = calloc(traits_len + 1, sizeof(struct hb_trait *));

			jv_object_foreach(traits, name, trait) {
				s->traits[trait_index] = calloc(1, sizeof(struct hb_trait));
				s->traits[trait_index]->name = strdup(jv_string_value(name));
				if (_hb_jv_parse_trait(trait, s->traits[trait_index++]) < 0)
					goto err;
			}
		} else if (traits_kind == JV_KIND_INVALID) {
			s->traits = calloc(1, sizeof(struct hb_trait *));
		} else {
			goto err;
		}
	}

	/////////////vertexes
	{
		jv vertexes;
		jv_kind vertexes_kind;
		int vertexes_len;

		vertexes = jv_object_get(jv_copy(root), jv_string("vertexes"));
		vertexes_kind = jv_get_kind(vertexes);

		if (vertexes_kind == JV_KIND_ARRAY) {
			vertexes_len = jv_array_length(jv_copy(vertexes));
			s->vertexes = calloc(vertexes_len + 1, sizeof(struct hb_vertex *));

			jv_array_foreach(vertexes, index, vertex) {
				s->vertexes[index] = calloc(1, sizeof(struct hb_vertex));
				if (_hb_jv_parse_vertex(vertex, s->vertexes[index], s->traits) < 0)
					goto err;
			}
		} else if (vertexes_kind == JV_KIND_INVALID) {
			s->vertexes = calloc(1, sizeof(struct hb_vertex *));
		} else {
			goto err;
		}
	}

	jv_free(root);
	return s;

err:
	hb_stadium_free(s);
	jv_free(root);

	return NULL;
}

extern struct hb_stadium *
hb_stadium_from_file(const char *file)
{
	FILE *fp;
	long file_size;
	char *raw_data;
	struct hb_stadium *s;

	fp = fopen(file, "r");
	file_size = _get_file_size(fp);
	raw_data = malloc(file_size + 1);

	fread(raw_data, 1, file_size, fp);
	raw_data[file_size] = '\0';
	s = hb_stadium_parse(raw_data);

	fclose(fp);
	free(raw_data);

	return s;
}

extern void
hb_stadium_free(struct hb_stadium *s)
{
	if (s->name)
		free(s->name);
	if (s->bg)
		free(s->bg);
	free(s);
}

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

extern const char *
hb_stadium_print(struct hb_stadium *s)
{
	struct hb_trait **trait;
	struct hb_vertex **vertex;

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

	for (trait = s->traits; *trait; ++trait)
		_hb_trait_print(*trait);

	for (vertex = s->vertexes; *vertex; ++vertex)
		_hb_vertex_print(vertex - s->vertexes, *vertex);
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
