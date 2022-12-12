#include <hb/stadium.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <jv.h>

static const float      HB_F_ZERO         = 0.0f;
static const bool       HB_B_TRUE         = true;

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
		if (!strcmp(type_str, "none") || !strcmp(type_str, "")) bg->type = HB_BACKGROUND_TYPE_NONE;
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
		uint32_t fallback_color;
		fallback_color = 0xff718c5a;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color(color, &bg->color, &fallback_color) < 0)
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
	else if (!strcmp(str, "")) *flag = 0;
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
	if (jv_get_kind(from) != JV_KIND_OBJECT)
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

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

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
		// FIXME: is bCoef required?
		// FIXME: is bCoef required?
		// FIXME: is bCoef required?
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
		// FIXME: is cGroup fallback/default value 0?
		// FIXME: is cGroup fallback/default value 0?
		// FIXME: is cGroup fallback/default value 0?
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
		// FIXME: is cMask fallback/default value 0?
		// FIXME: is cMask fallback/default value 0?
		// FIXME: is cMask fallback/default value 0?
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

static int
_hb_jv_parse_segment(jv from, struct hb_segment *to,
		struct hb_vertex **vertexes, struct hb_trait **traits)
{
	struct hb_trait *segment_trait;
	int num_vertexes;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	num_vertexes = 0;
	while (*vertexes) {
		++num_vertexes;
		++vertexes;
	}

	/////////////v0
	{
		jv v0;
		float f;
		v0 = jv_object_get(jv_copy(from), jv_string("v0"));
		if (_hb_jv_parse_number(v0, &f, NULL) < 0)
			return -1;
		to->v0 = f;
		if (to->v0 < 0 || to->v0 >= num_vertexes)
			return -1;
	}

	/////////////v1
	{
		jv v1;
		float f;
		v1 = jv_object_get(jv_copy(from), jv_string("v1"));
		if (_hb_jv_parse_number(v1, &f, NULL) < 0)
			return -1;
		to->v1 = f;
		if (to->v1 < 0 || to->v1 >= num_vertexes)
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
			segment_trait = NULL;
		else if (_hb_get_trait(traits, &segment_trait, trait_name) < 0) {
			free(trait_name);
			return -1;
		}
		free(trait_name);
	}

	/////////////bCoef
	{
		// FIXME: is bCoef required?
		// FIXME: is bCoef required?
		// FIXME: is bCoef required?
		jv b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (jv_get_kind(b_coef) == JV_KIND_INVALID) {
			if (NULL == segment_trait || !segment_trait->has_b_coef)
				return -1;
			to->b_coef = segment_trait->b_coef;
		} else {
			if (_hb_jv_parse_number(b_coef, &to->b_coef, NULL) < 0)
				return -1;
		}
	}

	/////////////curve
	{
		jv curve;
		curve = jv_object_get(jv_copy(from), jv_string("curve"));
		if (_hb_jv_parse_number(curve, &to->curve, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////bias
	{
		jv bias;
		bias = jv_object_get(jv_copy(from), jv_string("bias"));
		if (_hb_jv_parse_number(bias, &to->bias, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////cGroup
	{
		// FIXME: is cGroup fallback/default value 0?
		// FIXME: is cGroup fallback/default value 0?
		// FIXME: is cGroup fallback/default value 0?
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = 0;
		if (NULL != segment_trait && segment_trait->has_c_group)
			fallback_c_group = segment_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags(c_group, &to->c_group, &fallback_c_group) < 0)
			return -1;
	}

	/////////////cMask
	{
		// FIXME: is cMask fallback/default value 0?
		// FIXME: is cMask fallback/default value 0?
		// FIXME: is cMask fallback/default value 0?
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = 0;
		if (NULL != segment_trait && segment_trait->has_c_mask)
			fallback_c_mask = segment_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags(c_mask, &to->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////vis
	{
		jv vis;
		bool fallback_vis;
		fallback_vis = true;
		if (NULL != segment_trait && segment_trait->has_vis)
			fallback_vis = segment_trait->vis;
		vis = jv_object_get(jv_copy(from), jv_string("vis"));
		if (_hb_jv_parse_boolean(vis, &to->vis, &fallback_vis) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xff000000;
		if (NULL != segment_trait && segment_trait->has_color)
			fallback_color = segment_trait->color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color(color, &to->color, &fallback_color) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_team(jv from, enum hb_team *to, enum hb_team *fallback)
{
	const char *team_str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		team_str = jv_string_value(from);
		if (!strcmp(team_str, "red")) *to = HB_TEAM_RED;
		else if (!strcmp(team_str, "blue")) *to = HB_TEAM_BLUE;
		else if (!strcmp(team_str, "spect")) *to = HB_TEAM_SPECTATOR;
		else return -1;
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
_hb_jv_parse_goal(jv from, struct hb_goal *to)
{
	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	/////////////p0
	{
		jv p0;
		p0 = jv_object_get(jv_copy(from), jv_string("p0"));
		if (jv_get_kind(p0) != JV_KIND_ARRAY ||
				jv_array_length(p0) != 2)
			return -1;
		jv_array_foreach(p0, index, v) {
			if (_hb_jv_parse_number(v, &to->p0[index], NULL) < 0)
				return -1;
		}
	}

	/////////////p1
	{
		jv p1;
		p1 = jv_object_get(jv_copy(from), jv_string("p1"));
		if (jv_get_kind(p1) != JV_KIND_ARRAY ||
				jv_array_length(p1) != 2)
			return -1;
		jv_array_foreach(p1, index, v) {
			if (_hb_jv_parse_number(v, &to->p1[index], NULL) < 0)
				return -1;
		}
	}

	/////////////team
	{
		jv team;
		team = jv_object_get(jv_copy(from), jv_string("team"));
		if (_hb_jv_parse_team(team, &to->team, NULL) < 0 ||
				to->team == HB_TEAM_SPECTATOR)
			return -1;
	}
}

static int
_hb_jv_parse_disc(jv from, struct hb_disc *to,
		struct hb_trait **traits)
{
	struct hb_trait *disc_trait;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	/////////////pos
	{
		jv pos;
		jv_kind pos_kind;
		pos = jv_object_get(jv_copy(from), jv_string("pos"));
		pos_kind = jv_get_kind(pos);
		if (pos_kind == JV_KIND_INVALID)
			to->pos[0] = to->pos[1] = 0.0f;
		else if (pos_kind != JV_KIND_ARRAY || jv_array_length(pos) != 2)
			return -1;
		else {
			jv_array_foreach(pos, index, v) {
				if (_hb_jv_parse_number(v, &to->pos[index], NULL) < 0)
					return -1;
			}
		}
	}

	/////////////speed
	{
		jv speed;
		jv_kind speed_kind;
		speed = jv_object_get(jv_copy(from), jv_string("speed"));
		speed_kind = jv_get_kind(speed);
		if (speed_kind == JV_KIND_INVALID)
			to->speed[0] = to->speed[1] = 0.0f;
		else if (speed_kind != JV_KIND_ARRAY || jv_array_length(speed) != 2)
			return -1;
		else {
			jv_array_foreach(speed, index, v) {
				if (_hb_jv_parse_number(v, &to->speed[index], NULL) < 0)
					return -1;
			}
		}
	}

	/////////////gravity
	{
		jv gravity;
		jv_kind gravity_kind;
		gravity = jv_object_get(jv_copy(from), jv_string("gravity"));
		gravity_kind = jv_get_kind(gravity);
		if (gravity_kind == JV_KIND_INVALID)
			to->gravity[0] = to->gravity[1] = 0.0f;
		else if (gravity_kind != JV_KIND_ARRAY || jv_array_length(gravity) != 2)
			return -1;
		else {
			jv_array_foreach(gravity, index, v) {
				if (_hb_jv_parse_number(v, &to->gravity[index], NULL) < 0)
					return -1;
			}
		}
	}

	/////////////trait
	{
		jv trait;
		char *trait_name;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_parse_string(trait, &trait_name, "__no_trait__") < 0)
			return -1;
		if (!strcmp(trait_name, "__no_trait__"))
			disc_trait = NULL;
		else if (_hb_get_trait(traits, &disc_trait, trait_name) < 0) {
			free(trait_name);
			return -1;
		}
		free(trait_name);
	}

	/////////////radius
	{
		jv radius;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));

		if (jv_get_kind(radius) == JV_KIND_INVALID &&
				disc_trait != NULL && disc_trait->has_radius) {
			to->radius = disc_trait->radius;
		} else if (_hb_jv_parse_number(radius, &to->radius, NULL) < 0) {
			return -1;
		}
	}

	/////////////invMass
	{
		jv inv_mass;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));

		if (jv_get_kind(inv_mass) == JV_KIND_INVALID &&
				disc_trait != NULL && disc_trait->has_inv_mass) {
			to->inv_mass = disc_trait->inv_mass;
		} else if (_hb_jv_parse_number(inv_mass, &to->inv_mass, NULL) < 0) {
			return -1;
		}
	}

	/////////////damping
	{
		jv damping;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (_hb_jv_parse_number(damping, &to->damping, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xffffffff;
		if (NULL != disc_trait && disc_trait->has_color)
			fallback_color = disc_trait->color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color(color, &to->color, &fallback_color) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (jv_get_kind(b_coef) == JV_KIND_INVALID) {
			if (NULL == disc_trait || !disc_trait->has_b_coef)
				return -1;
			to->b_coef = disc_trait->b_coef;
		} else {
			if (_hb_jv_parse_number(b_coef, &to->b_coef, NULL) < 0)
				return -1;
		}
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = 0;
		if (NULL != disc_trait && disc_trait->has_c_mask)
			fallback_c_mask = disc_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags(c_mask, &to->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = 0;
		if (NULL != disc_trait && disc_trait->has_c_group)
			fallback_c_group = disc_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags(c_group, &to->c_group, &fallback_c_group) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_plane(jv from, struct hb_plane *to, struct hb_trait **traits)
{
	struct hb_trait *plane_trait;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	/////////////normal
	{
		jv normal;
		normal = jv_object_get(jv_copy(from), jv_string("normal"));
		if (jv_get_kind(normal) != JV_KIND_ARRAY ||
				jv_array_length(normal) != 2)
			return -1;
		jv_array_foreach(normal, index, v) {
			if (_hb_jv_parse_number(v, &to->normal[index], NULL) < 0)
				return -1;
		}
	}

	/////////////dist
	{
		jv dist;
		dist = jv_object_get(jv_copy(from), jv_string("dist"));
		if (_hb_jv_parse_number(dist, &to->dist, NULL) < 0)
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
			plane_trait = NULL;
		else if (_hb_get_trait(traits, &plane_trait, trait_name) < 0) {
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
			if (NULL == plane_trait || !plane_trait->has_b_coef)
				return -1;
			to->b_coef = plane_trait->b_coef;
		} else {
			if (_hb_jv_parse_number(b_coef, &to->b_coef, NULL) < 0)
				return -1;
		}
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = 0;
		if (NULL != plane_trait && plane_trait->has_c_mask)
			fallback_c_mask = plane_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags(c_mask, &to->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = 0;
		if (NULL != plane_trait && plane_trait->has_c_group)
			fallback_c_group = plane_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags(c_group, &to->c_group, &fallback_c_group) < 0)
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

	/////////////segments
	{
		jv segments;
		jv_kind segments_kind;
		int segments_len;

		segments = jv_object_get(jv_copy(root), jv_string("segments"));
		segments_kind = jv_get_kind(segments);

		if (segments_kind == JV_KIND_ARRAY) {
			segments_len = jv_array_length(jv_copy(segments));
			s->segments = calloc(segments_len + 1, sizeof(struct hb_segment *));

			jv_array_foreach(segments, index, segment) {
				s->segments[index] = calloc(1, sizeof(struct hb_segment));
				if (_hb_jv_parse_segment(segment, s->segments[index], s->vertexes, s->traits) < 0)
					goto err;
			}
		} else if (segments_kind == JV_KIND_INVALID) {
			s->segments = calloc(1, sizeof(struct hb_segment *));
		} else {
			goto err;
		}
	}

	/////////////goals
	{
		jv goals;
		jv_kind goals_kind;
		int goals_len;

		goals = jv_object_get(jv_copy(root), jv_string("goals"));
		goals_kind = jv_get_kind(goals);

		if (goals_kind == JV_KIND_ARRAY) {
			goals_len = jv_array_length(jv_copy(goals));
			s->goals = calloc(goals_len + 1, sizeof(struct hb_goal *));

			jv_array_foreach(goals, index, goal) {
				s->goals[index] = calloc(1, sizeof(struct hb_goal));
				if (_hb_jv_parse_goal(goal, s->goals[index]) < 0)
					goto err;
			}
		} else if (goals_kind == JV_KIND_INVALID) {
			s->goals = calloc(1, sizeof(struct hb_goal *));
		} else {
			goto err;
		}
	}

	/////////////ballPhysics
	{
		jv ball_physics;
		jv_kind ball_physics_kind;
		char *ball_physics_str;

		ball_physics = jv_object_get(jv_copy(root), jv_string("ballPhysics"));
		ball_physics_kind = jv_get_kind(ball_physics);

		if (ball_physics_kind == JV_KIND_OBJECT) {
			s->ball_physics = calloc(1, sizeof(struct hb_disc));
			if (_hb_jv_parse_disc(ball_physics, s->ball_physics, s->traits) < 0)
				goto err;
		} else if (ball_physics_kind == JV_KIND_STRING) {
			if (_hb_jv_parse_string(ball_physics, &ball_physics_str, NULL) < 0)
				goto err;
			if (!strcmp(ball_physics_str, "disc0"))
				s->ball_physics = NULL;
			else goto err;
		} else if (ball_physics_kind == JV_KIND_INVALID) {
			s->ball_physics = calloc(1, sizeof(struct hb_disc));
			s->ball_physics->radius = 10.0f;
			s->ball_physics->b_coef = 0.5f;
			s->ball_physics->inv_mass = 1.0f;
			s->ball_physics->damping = 0.99f;
			s->ball_physics->color = 0xffffffff;
			s->ball_physics->c_mask = HB_COLLISION_ALL;
			s->ball_physics->c_group = HB_COLLISION_BALL;
			s->ball_physics->speed[0] = s->ball_physics->speed[1] = 0.0f;
		} else {
			goto err;
		}
	}

	/////////////discs
	{
		jv discs;
		jv_kind discs_kind;
		int discs_len;
		int disc_index;

		discs = jv_object_get(jv_copy(root), jv_string("discs"));
		discs_kind = jv_get_kind(discs);

		if (discs_kind == JV_KIND_ARRAY) {
			discs_len = jv_array_length(jv_copy(discs));
			if (s->ball_physics != NULL) discs_len++;
			s->discs = calloc(discs_len + 1, sizeof(struct hb_disc *));
			if (s->ball_physics != NULL) s->discs[0] = s->ball_physics;

			jv_array_foreach(discs, index, disc) {
				disc_index = index;
				if (s->ball_physics != NULL) disc_index++;
				s->discs[disc_index] = calloc(1, sizeof(struct hb_disc));
				if (_hb_jv_parse_disc(disc, s->discs[disc_index], s->traits) < 0)
					goto err;
			}
		} else if (discs_kind == JV_KIND_INVALID) {
			if (s->ball_physics == NULL)
				goto err;
			s->discs = calloc(2, sizeof(struct hb_disc *));
			s->discs[0] = s->ball_physics;
		} else {
			goto err;
		}
	}

	/////////////planes
	{
		jv planes;
		jv_kind planes_kind;
		int planes_len;

		planes = jv_object_get(jv_copy(root), jv_string("planes"));
		planes_kind = jv_get_kind(planes);

		if (planes_kind == JV_KIND_ARRAY) {
			planes_len = jv_array_length(jv_copy(planes));
			s->planes = calloc(planes_len + 1, sizeof(struct hb_plane *));

			jv_array_foreach(planes, index, plane) {
				s->planes[index] = calloc(1, sizeof(struct hb_plane));
				if (_hb_jv_parse_plane(plane, s->planes[index], s->traits) < 0)
					goto err;
			}
		} else if (planes_kind == JV_KIND_INVALID) {
			s->planes = calloc(1, sizeof(struct hb_plane *));
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

	if (NULL == (fp = fopen(file, "r")))
		return NULL;

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
