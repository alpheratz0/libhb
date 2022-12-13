#include <hb/stadium.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <jv.h>

static const float      HB_F_ZERO         = 0.0f;
static const bool       HB_B_TRUE         = true;
static const float     *HB_V2_ZERO        = (const float [2]){ 0.0f, 0.0f };

static int _hb_jv_parse_string(jv from, char **to, const char *fallback);
static int _hb_jv_parse_number(jv from, float *to, const float *fallback);
static int _hb_jv_parse_boolean(jv from, bool *to, const bool *fallback);
static int _hb_jv_parse_camera_follow(jv from, enum hb_camera_follow *to, enum hb_camera_follow *fallback);
static int _hb_jv_parse_kick_off_reset(jv from, enum hb_kick_off_reset *to, enum hb_kick_off_reset *fallback);
static int _hb_jv_parse_bg_type(jv from, enum hb_background_type *to, enum hb_background_type *fallback);
static int _hb_jv_parse_color(jv from, uint32_t *to, uint32_t *fallback);
static int _hb_jv_parse_bg(jv from, struct hb_background **to);
static int _hb_jv_parse_collision_flag(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_collision_flags(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_trait(jv from, jv name, struct hb_trait **to);
static int _hb_jv_parse_trait_list(jv from, struct hb_trait ***to);
static int _hb_jv_parse_vertex(jv from, struct hb_vertex **to, struct hb_trait **traits);
static int _hb_jv_parse_vertex_list(jv from, struct hb_vertex ***to, struct hb_trait **traits);
static int _hb_jv_parse_segment(jv from, struct hb_segment **to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_segment_list(jv from, struct hb_segment ***to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_vec2(jv from, float to[2], const float *(fallback[2]));
static int _hb_jv_parse_team(jv from, enum hb_team *to, enum hb_team *fallback);
static int _hb_jv_parse_goal(jv from, struct hb_goal **to);
static int _hb_jv_parse_goal_list(jv from, struct hb_goal ***to);
static int _hb_jv_parse_ball_physics(jv from, struct hb_disc **to);
static int _hb_jv_parse_disc(jv from, struct hb_disc **to, struct hb_trait **traits);
static int _hb_jv_parse_disc_list(jv from, struct hb_disc ***to, struct hb_trait **traits, struct hb_disc **ball_physics);

static int _hb_jv_parse_string_and_free(jv from, char **to, const char *fallback);
static int _hb_jv_parse_number_and_free(jv from, float *to, const float *fallback);
static int _hb_jv_parse_boolean_and_free(jv from, bool *to, const bool *fallback);
static int _hb_jv_parse_camera_follow_and_free(jv from, enum hb_camera_follow *to, enum hb_camera_follow *fallback);
static int _hb_jv_parse_kick_off_reset_and_free(jv from, enum hb_kick_off_reset *to, enum hb_kick_off_reset *fallback);
static int _hb_jv_parse_bg_type_and_free(jv from, enum hb_background_type *to, enum hb_background_type *fallback);
static int _hb_jv_parse_color_and_free(jv from, uint32_t *to, uint32_t *fallback);
static int _hb_jv_parse_bg_and_free(jv from, struct hb_background **to);
static int _hb_jv_parse_collision_flag_and_free(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_collision_flags_and_free(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_trait_and_free(jv from, jv name, struct hb_trait **to);
static int _hb_jv_parse_trait_list_and_free(jv from, struct hb_trait ***to);
static int _hb_jv_parse_vertex_and_free(jv from, struct hb_vertex **to, struct hb_trait **traits);
static int _hb_jv_parse_vertex_list_and_free(jv from, struct hb_vertex ***to, struct hb_trait **traits);
static int _hb_jv_parse_segment_and_free(jv from, struct hb_segment **to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_segment_list_and_free(jv from, struct hb_segment ***to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_vec2_and_free(jv from, float to[2], const float *(fallback[]));
static int _hb_jv_parse_team_and_free(jv from, enum hb_team *to, enum hb_team *fallback);
static int _hb_jv_parse_goal_and_free(jv from, struct hb_goal **to);
static int _hb_jv_parse_goal_list_and_free(jv from, struct hb_goal ***to);
static int _hb_jv_parse_ball_physics_and_free(jv from, struct hb_disc **to);
static int _hb_jv_parse_disc_and_free(jv from, struct hb_disc **to, struct hb_trait **traits);
static int _hb_jv_parse_disc_list_and_free(jv from, struct hb_disc ***to, struct hb_trait **traits, struct hb_disc **ball_physics);

static int
_hb_trait_find_by_name(struct hb_trait **traits, struct hb_trait **trait, const char *name)
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

//////////////////////////////////////////////
//////////////////PARSE///////////////////////
//////////////////////////////////////////////

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
_hb_jv_parse_camera_follow(jv from, enum hb_camera_follow *to,
		enum hb_camera_follow *fallback)
{
	const char *camera_follow_str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		camera_follow_str = jv_string_value(from);
		if (!strcmp(camera_follow_str, "ball")) *to = HB_CAMERA_FOLLOW_BALL;
		else if (!strcmp(camera_follow_str, "player")) *to = HB_CAMERA_FOLLOW_PLAYER;
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
_hb_jv_parse_kick_off_reset(jv from, enum hb_kick_off_reset *to,
		enum hb_kick_off_reset *fallback)
{
	const char *kick_off_reset_str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		kick_off_reset_str = jv_string_value(from);
		if (!strcmp(kick_off_reset_str, "partial")) *to = HB_KICK_OFF_RESET_PARTIAL;
		else if (!strcmp(kick_off_reset_str, "full")) *to = HB_KICK_OFF_RESET_FULL;
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
_hb_jv_parse_bg_type(jv from, enum hb_background_type *to,
		enum hb_background_type *fallback)
{
	const char *type_str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		type_str = jv_string_value(from);
		if (!strcmp(type_str, "grass")) *to = HB_BACKGROUND_TYPE_GRASS;
		else if (!strcmp(type_str, "hockey")) *to = HB_BACKGROUND_TYPE_HOCKEY;
		else *to = HB_BACKGROUND_TYPE_NONE;
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
_hb_jv_parse_color(jv from, uint32_t *to, uint32_t *fallback)
{
	jv_kind kind;
	const char *color_str;
	char *color_str_end;

	kind = jv_get_kind(from);

	if (kind == JV_KIND_INVALID) {
		if (NULL == fallback)
			return -1;
		*to = *fallback;
		return 0;
	}

	if (kind == JV_KIND_STRING) {
		color_str = jv_string_value(from);
		if (!strcmp(color_str, "transparent")) *to = 0x00000000;
		else {
			*to = strtol(color_str, &color_str_end, 16);
			if (color_str_end - color_str > 8 ||
					color_str_end[0] != '\0')
				return -1;
		}
		return 0;
	}

	if (kind == JV_KIND_ARRAY) {
		if (jv_array_length(jv_copy(from)) != 3)
			return -1;
		*to = 0xff << 24;
		jv_array_foreach(from, index, value) {
			if (jv_get_kind(value) != JV_KIND_NUMBER)
				return -1;
			*to |= ((int)(jv_number_value(value)) & 0xff) << (8*(2-index));
			jv_free(value);
		}
		return 0;
	}

	return -1;
}

static int
_hb_jv_parse_bg(jv from, struct hb_background **to)
{
	struct hb_background *bg;
	jv_kind kind;

	kind = jv_get_kind(from);
	bg = *to = malloc(sizeof(struct hb_background));

	if (kind == JV_KIND_INVALID) {
		bg->type = HB_BACKGROUND_TYPE_NONE;
		bg->width = 0.0f;
		bg->height = 0.0f;
		bg->kick_off_radius = 0.0f;
		bg->corner_radius = 0.0f;
		bg->goal_line = 0.0f;
		bg->color = 0xff718c5a;
		return 0;
	}

	if (kind != JV_KIND_OBJECT)
		return -1;

	/////////////type
	{
		jv type;
		enum hb_background_type fallback_type;
		fallback_type = HB_BACKGROUND_TYPE_NONE;
		type = jv_object_get(jv_copy(from), jv_string("type"));
		if (_hb_jv_parse_bg_type_and_free(type, &bg->type, &fallback_type) < 0)
			return -1;
	}

	/////////////width
	{
		jv width;
		width = jv_object_get(jv_copy(from), jv_string("width"));
		if (_hb_jv_parse_number_and_free(width, &bg->width, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////height
	{
		jv height;
		height = jv_object_get(jv_copy(from), jv_string("height"));
		if (_hb_jv_parse_number_and_free(height, &bg->height, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////kickOffRadius
	{
		jv kick_off_radius;
		kick_off_radius = jv_object_get(jv_copy(from), jv_string("kickOffRadius"));
		if (_hb_jv_parse_number_and_free(kick_off_radius, &bg->kick_off_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////cornerRadius
	{
		jv corner_radius;
		corner_radius = jv_object_get(jv_copy(from), jv_string("cornerRadius"));
		if (_hb_jv_parse_number_and_free(corner_radius, &bg->corner_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////goalLine
	{
		jv goal_line;
		goal_line = jv_object_get(jv_copy(from), jv_string("goalLine"));
		if (_hb_jv_parse_number_and_free(goal_line, &bg->goal_line, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xff718c5a;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &bg->color, &fallback_color) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_collision_flag(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	const char *str;

	if (jv_get_kind(from) == JV_KIND_INVALID) {
		if (NULL == fallback)
			return -1;
		*to = *fallback;
		return 0;
	}

	if (jv_get_kind(from) != JV_KIND_STRING)
		return -1;

	str = jv_string_value(from);
	if (!strcmp(str, "ball")) *to = HB_COLLISION_BALL;
	else if (!strcmp(str, "red")) *to = HB_COLLISION_RED;
	else if (!strcmp(str, "blue")) *to = HB_COLLISION_BLUE;
	else if (!strcmp(str, "redKO")) *to = HB_COLLISION_RED_KO;
	else if (!strcmp(str, "blueKO")) *to = HB_COLLISION_BLUE_KO;
	else if (!strcmp(str, "wall")) *to = HB_COLLISION_WALL;
	else if (!strcmp(str, "all")) *to = HB_COLLISION_ALL;
	else if (!strcmp(str, "kick")) *to = HB_COLLISION_KICK;
	else if (!strcmp(str, "score")) *to = HB_COLLISION_SCORE;
	else if (!strcmp(str, "c0")) *to = HB_COLLISION_C0;
	else if (!strcmp(str, "c1")) *to = HB_COLLISION_C1;
	else if (!strcmp(str, "c2")) *to = HB_COLLISION_C2;
	else if (!strcmp(str, "c3")) *to = HB_COLLISION_C3;
	else *to = 0;
	return 0;
}

static int
_hb_jv_parse_collision_flags(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	enum hb_collision_flags flag;
	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		jv_array_foreach(from, index, value) {
			if (_hb_jv_parse_collision_flag_and_free(value, &flag, NULL) < 0)
				return -1;
			*to |= flag;
		}
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
_hb_jv_parse_trait(jv from, jv name, struct hb_trait **to)
{
	struct hb_trait *trait;

	if (jv_get_kind(name) != JV_KIND_STRING ||
			jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	trait = *to = calloc(1, sizeof(struct hb_trait));
	trait->name = strdup(jv_string_value(name));

	/////////////curve
	{
		jv curve;
		curve = jv_object_get(jv_copy(from), jv_string("curve"));
		if (jv_get_kind(curve) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(curve, &trait->curve, NULL) < 0)
				return -1;
			trait->has_curve = true;
		}
	}

	/////////////damping
	{
		jv damping;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (jv_get_kind(damping) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(damping, &trait->damping, NULL) < 0)
				return -1;
			trait->has_damping = true;
		}
	}

	/////////////invMass
	{
		jv inv_mass;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (jv_get_kind(inv_mass) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(inv_mass, &trait->inv_mass, NULL) < 0)
				return -1;
			trait->has_inv_mass = true;
		}
	}

	/////////////radius
	{
		jv radius;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (jv_get_kind(radius) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(radius, &trait->radius, NULL) < 0)
				return -1;
			trait->has_radius = true;
		}
	}

	/////////////bCoef
	{
		jv b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (jv_get_kind(b_coef) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(b_coef, &trait->b_coef, NULL) < 0)
				return -1;
			trait->has_b_coef = true;
		}
	}

	/////////////color
	{
		jv color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (jv_get_kind(color) != JV_KIND_INVALID) {
			if (_hb_jv_parse_color_and_free(color, &trait->color, NULL) < 0)
				return -1;
			trait->has_color = true;
		}
	}

	/////////////vis
	{
		jv vis;
		vis = jv_object_get(jv_copy(from), jv_string("vis"));
		if (jv_get_kind(vis) != JV_KIND_INVALID) {
			if (_hb_jv_parse_boolean_and_free(vis, &trait->vis, NULL) < 0)
				return -1;
			trait->has_vis = true;
		}
	}

	/////////////cGroup
	{
		jv c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (jv_get_kind(c_group) != JV_KIND_INVALID) {
			if (_hb_jv_parse_collision_flags_and_free(c_group, &trait->c_group, NULL) < 0)
				return -1;
			trait->has_c_group = true;
		}
	}

	/////////////cMask
	{
		jv c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (jv_get_kind(c_mask) != JV_KIND_INVALID) {
			if (_hb_jv_parse_collision_flags_and_free(c_mask, &trait->c_mask, NULL) < 0)
				return -1;
			trait->has_c_mask = true;
		}
	}

	return 0;
}

static int
_hb_jv_parse_trait_list(jv from, struct hb_trait ***to)
{
	int count, index;

	switch (jv_get_kind(from)) {
	case JV_KIND_OBJECT:
		index = 0;
		count = jv_object_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_trait *));
		jv_object_foreach(from, name, trait) {
			if (_hb_jv_parse_trait_and_free(trait, name, &((*to)[index++])) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_trait *));
		break;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_vertex(jv from, struct hb_vertex **to,
		struct hb_trait **traits)
{
	struct hb_vertex *vert;
	struct hb_trait *vert_trait;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	vert = *to = calloc(1, sizeof(struct hb_vertex));

	/////////////x
	{
		jv x;
		x = jv_object_get(jv_copy(from), jv_string("x"));
		if (_hb_jv_parse_number_and_free(x, &vert->x, NULL) < 0)
			return -1;
	}

	/////////////y
	{
		jv y;
		y = jv_object_get(jv_copy(from), jv_string("y"));
		if (_hb_jv_parse_number_and_free(y, &vert->y, NULL) < 0)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		char *trait_name;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_parse_string_and_free(trait, &trait_name, "__no_trait__") < 0)
			return -1;
		if (!strcmp(trait_name, "__no_trait__") ||
				_hb_trait_find_by_name(traits, &vert_trait, trait_name) < 0)
			vert_trait = NULL;
		free(trait_name);
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 0.0f;
		if (NULL != vert_trait && vert_trait->has_b_coef)
			fallback_b_coef = vert_trait->b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &vert->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////cGroup
	{
		// FIXME: is cGroup fallback/default value 0?
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = 0;
		if (NULL != vert_trait && vert_trait->has_c_group)
			fallback_c_group = vert_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &vert->c_group, &fallback_c_group) < 0)
			return -1;
	}

	/////////////cMask
	{
		// FIXME: is cMask fallback/default value 0?
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = 0;
		if (NULL != vert_trait && vert_trait->has_c_mask)
			fallback_c_mask = vert_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &vert->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_vertex_list(jv from, struct hb_vertex ***to,
		struct hb_trait **traits)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_vertex *));
		jv_array_foreach(from, index, vertex) {
			if (_hb_jv_parse_vertex_and_free(vertex, &((*to)[index]), traits) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_vertex *));
		break;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_segment(jv from, struct hb_segment **to,
		struct hb_vertex **vertexes, struct hb_trait **traits)
{
	struct hb_segment *segm;
	struct hb_trait *segm_trait;
	int vert_count;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	vert_count = 0;
	while (*vertexes) {
		++vert_count;
		++vertexes;
	}

	segm = *to = calloc(1, sizeof(struct hb_segment));

	/////////////v0
	{
		jv v0;
		float f;
		v0 = jv_object_get(jv_copy(from), jv_string("v0"));
		if (_hb_jv_parse_number_and_free(v0, &f, NULL) < 0)
			return -1;
		segm->v0 = f;
		if (segm->v0 < 0 || segm->v0 >= vert_count)
			return -1;
	}

	/////////////v1
	{
		jv v1;
		float f;
		v1 = jv_object_get(jv_copy(from), jv_string("v1"));
		if (_hb_jv_parse_number_and_free(v1, &f, NULL) < 0)
			return -1;
		segm->v1 = f;
		if (segm->v1 < 0 || segm->v1 >= vert_count)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		char *trait_name;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_parse_string_and_free(trait, &trait_name, "__no_trait__") < 0)
			return -1;
		if (!strcmp(trait_name, "__no_trait__") ||
				_hb_trait_find_by_name(traits, &segm_trait, trait_name) < 0)
			segm_trait = NULL;
		free(trait_name);
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 0.0f;
		if (NULL != segm_trait && segm_trait->has_b_coef)
			fallback_b_coef = segm_trait->b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &segm->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////curve
	{
		jv curve;
		curve = jv_object_get(jv_copy(from), jv_string("curve"));
		if (_hb_jv_parse_number_and_free(curve, &segm->curve, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////bias
	{
		jv bias;
		bias = jv_object_get(jv_copy(from), jv_string("bias"));
		if (_hb_jv_parse_number_and_free(bias, &segm->bias, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////cGroup
	{
		// FIXME: is cGroup fallback/default value 0?
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = 0;
		if (NULL != segm_trait && segm_trait->has_c_group)
			fallback_c_group = segm_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &segm->c_group, &fallback_c_group) < 0)
			return -1;
	}

	/////////////cMask
	{
		// FIXME: is cMask fallback/default value 0?
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = 0;
		if (NULL != segm_trait && segm_trait->has_c_mask)
			fallback_c_mask = segm_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &segm->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////vis
	{
		jv vis;
		bool fallback_vis;
		fallback_vis = true;
		if (NULL != segm_trait && segm_trait->has_vis)
			fallback_vis = segm_trait->vis;
		vis = jv_object_get(jv_copy(from), jv_string("vis"));
		if (_hb_jv_parse_boolean_and_free(vis, &segm->vis, &fallback_vis) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xff000000;
		if (NULL != segm_trait && segm_trait->has_color)
			fallback_color = segm_trait->color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &segm->color, &fallback_color) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_segment_list(jv from, struct hb_segment ***to,
		struct hb_vertex **vertexes, struct hb_trait **traits)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_segment *));
		jv_array_foreach(from, index, segment) {
			if (_hb_jv_parse_segment_and_free(segment, &((*to)[index]), vertexes, traits) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_segment *));
		break;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_vec2(jv from, float to[2], const float *(fallback[2]))
{
	switch (jv_get_kind(from)) {
		case JV_KIND_ARRAY:
			if (jv_array_length(jv_copy(from)) != 2)
				return -1;
			jv_array_foreach(from, index, v) {
				if (_hb_jv_parse_number_and_free(v, &to[index], NULL) < 0)
					return -1;
			}
			return 0;
		case JV_KIND_INVALID:
			if (fallback == NULL)
				return -1;
			to[0] = (*fallback)[0];
			to[1] = (*fallback)[1];
			return 0;
		default:
			return -1;
	}
}

static int
_hb_jv_parse_team(jv from, enum hb_team *to, enum hb_team *fallback)
{
	const char *str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		str = jv_string_value(from);
		if (!strcmp(str, "red")) *to = HB_TEAM_RED;
		else if (!strcmp(str, "blue")) *to = HB_TEAM_BLUE;
		else if (!strcmp(str, "spect")) *to = HB_TEAM_SPECTATOR;
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
_hb_jv_parse_goal(jv from, struct hb_goal **to)
{
	struct hb_goal *goal;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	goal = *to = calloc(1, sizeof(struct hb_goal));

	/////////////p0
	{
		jv p0;
		p0 = jv_object_get(jv_copy(from), jv_string("p0"));
		if (_hb_jv_parse_vec2_and_free(p0, goal->p0, NULL) < 0)
			return -1;
	}

	/////////////p1
	{
		jv p1;
		p1 = jv_object_get(jv_copy(from), jv_string("p1"));
		if (_hb_jv_parse_vec2_and_free(p1, goal->p1, NULL) < 0)
			return -1;
	}

	/////////////team
	{
		jv team;
		team = jv_object_get(jv_copy(from), jv_string("team"));
		if (_hb_jv_parse_team_and_free(team, &goal->team, NULL) < 0 ||
				goal->team == HB_TEAM_SPECTATOR)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_goal_list(jv from, struct hb_goal ***to)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_goal *));
		jv_array_foreach(from, index, goal) {
			if (_hb_jv_parse_goal_and_free(goal, &((*to)[index])) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_goal *));
		break;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_ball_physics(jv from, struct hb_disc **to)
{
	const char *ball_physics_str;
	struct hb_disc *ball_physics;
	jv_kind kind;

	kind = jv_get_kind(from);

	if (kind != JV_KIND_OBJECT
			&& kind != JV_KIND_STRING
			&& kind != JV_KIND_INVALID)
		return -1;

	if (kind == JV_KIND_INVALID) {
		ball_physics = *to = calloc(1, sizeof(struct hb_disc));
		ball_physics->radius = 10.0f;
		ball_physics->b_coef = 0.5f;
		ball_physics->inv_mass = 1.0f;
		ball_physics->damping = 0.99f;
		ball_physics->color = 0xffffffff;
		ball_physics->c_mask = HB_COLLISION_ALL;
		ball_physics->c_group = HB_COLLISION_BALL;
		ball_physics->speed[0] = ball_physics->speed[1] = 0.0f;
		return 0;
	}

	if (kind == JV_KIND_STRING) {
		ball_physics_str = jv_string_value(from);
		if (strcmp(ball_physics_str, "disc0")) return -1;
		*to = NULL;
		return 0;
	}

	ball_physics = *to = calloc(1, sizeof(struct hb_disc));

	/////////////pos
	{
		jv pos;
		pos = jv_object_get(jv_copy(from), jv_string("pos"));
		if (_hb_jv_parse_vec2_and_free(pos, ball_physics->pos, &HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////speed
	{
		jv speed;
		speed = jv_object_get(jv_copy(from), jv_string("speed"));
		if (_hb_jv_parse_vec2_and_free(speed, ball_physics->speed, &HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////gravity
	{
		jv gravity;
		gravity = jv_object_get(jv_copy(from), jv_string("gravity"));
		if (_hb_jv_parse_vec2_and_free(gravity, ball_physics->gravity, &HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////radius
	{
		jv radius;
		float fallback_radius;
		fallback_radius = 10.0f;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (_hb_jv_parse_number_and_free(radius, &ball_physics->radius, &fallback_radius) < 0)
			return -1;
	}

	/////////////invMass
	{
		jv inv_mass;
		float fallback_inv_mass;
		fallback_inv_mass = 1.0f;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (_hb_jv_parse_number_and_free(inv_mass, &ball_physics->inv_mass, &fallback_inv_mass) < 0)
			return -1;
	}

	/////////////damping
	{
		jv damping;
		float fallback_damping;
		fallback_damping = 0.99f;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (_hb_jv_parse_number_and_free(damping, &ball_physics->damping, &fallback_damping) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xffffffff;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &ball_physics->color, &fallback_color) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 0.5f;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &ball_physics->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = HB_COLLISION_ALL;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &ball_physics->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = HB_COLLISION_KICK | HB_COLLISION_SCORE;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &ball_physics->c_group, &fallback_c_group) < 0)
			return -1;
		ball_physics->c_group |= HB_COLLISION_KICK | HB_COLLISION_SCORE;
	}

	return 0;
}

static int
_hb_jv_parse_disc(jv from, struct hb_disc **to,
		struct hb_trait **traits)
{
	struct hb_disc *disc;
	struct hb_trait *disc_trait;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	disc = *to = calloc(1, sizeof(struct hb_disc));

	/////////////pos
	{
		jv pos;
		pos = jv_object_get(jv_copy(from), jv_string("pos"));
		if (_hb_jv_parse_vec2_and_free(pos, disc->pos, &HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////speed
	{
		jv speed;
		speed = jv_object_get(jv_copy(from), jv_string("speed"));
		if (_hb_jv_parse_vec2_and_free(speed, disc->speed, &HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////gravity
	{
		jv gravity;
		gravity = jv_object_get(jv_copy(from), jv_string("gravity"));
		if (_hb_jv_parse_vec2_and_free(gravity, disc->gravity, &HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		char *trait_name;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_parse_string_and_free(trait, &trait_name, "__no_trait__") < 0)
			return -1;
		if (!strcmp(trait_name, "__no_trait__") ||
				_hb_trait_find_by_name(traits, &disc_trait, trait_name) < 0)
			disc_trait = NULL;
		free(trait_name);
	}

	/////////////radius
	{
		jv radius;
		float fallback_radius;
		fallback_radius = 10.0f;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (disc_trait != NULL && disc_trait->has_radius)
			fallback_radius = disc_trait->radius;
		if (_hb_jv_parse_number_and_free(radius, &disc->radius, &fallback_radius) < 0)
			return -1;
	}

	/////////////invMass
	{
		jv inv_mass;
		float fallback_inv_mass;
		fallback_inv_mass = 1.0f;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (disc_trait != NULL && disc_trait->has_inv_mass)
			fallback_inv_mass = disc_trait->inv_mass;
		if (_hb_jv_parse_number_and_free(inv_mass, &disc->inv_mass, &fallback_inv_mass) < 0)
			return -1;
	}

	/////////////damping
	{
		jv damping;
		float fallback_damping;
		fallback_damping =  0.99;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (_hb_jv_parse_number_and_free(damping, &disc->damping, &fallback_damping) < 0)
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
		if (_hb_jv_parse_color_and_free(color, &disc->color, &fallback_color) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 0.5f;
		if (NULL != disc_trait && disc_trait->has_b_coef)
			fallback_b_coef = disc_trait->b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &disc->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = 0;
		if (NULL != disc_trait && disc_trait->has_c_mask)
			fallback_c_mask = disc_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &disc->c_mask, &fallback_c_mask) < 0)
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
		if (_hb_jv_parse_collision_flags_and_free(c_group, &disc->c_group, &fallback_c_group) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_disc_list(jv from, struct hb_disc ***to,
		struct hb_trait **traits, struct hb_disc **ball_physics)
{
	int count, disc_index;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		if (*ball_physics == NULL && count == 0) return -1;
		if (*ball_physics != NULL) ++count;
		*to = calloc(count + 1, sizeof(struct hb_disc *));
		if (*ball_physics != NULL) (*to)[0] = *ball_physics;
		jv_array_foreach(from, index, disc) {
			disc_index = index;
			if (*ball_physics != NULL) ++disc_index;
			if (_hb_jv_parse_disc_and_free(disc, &((*to)[disc_index]), traits) < 0)
				return -1;
		}
		if (*ball_physics == NULL) *ball_physics = (*to)[0];
		return 0;
	case JV_KIND_INVALID:
		if (ball_physics == NULL)
			return -1;
		*to = calloc(2, sizeof(struct hb_disc *));
		(*to)[0] = *ball_physics;
		return 0;
	default:
		return -1;
	}
}

//////////////////////////////////////////////
//////////////PARSE + JV_FREE/////////////////
//////////////////////////////////////////////

static int
_hb_jv_parse_string_and_free(jv from, char **to, const char *fallback)
{
	int ret;
	ret = _hb_jv_parse_string(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_number_and_free(jv from, float *to, const float *fallback)
{
	int ret;
	ret = _hb_jv_parse_number(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_boolean_and_free(jv from, bool *to, const bool *fallback)
{
	int ret;
	ret = _hb_jv_parse_boolean(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_camera_follow_and_free(jv from, enum hb_camera_follow *to,
		enum hb_camera_follow *fallback)
{
	int ret;
	ret = _hb_jv_parse_camera_follow(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_kick_off_reset_and_free(jv from, enum hb_kick_off_reset *to,
		enum hb_kick_off_reset *fallback)
{
	int ret;
	ret = _hb_jv_parse_kick_off_reset(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_bg_type_and_free(jv from, enum hb_background_type *to,
		enum hb_background_type *fallback)
{
	int ret;
	ret = _hb_jv_parse_bg_type(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_color_and_free(jv from, uint32_t *to, uint32_t *fallback)
{
	int ret;
	ret = _hb_jv_parse_color(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_bg_and_free(jv from, struct hb_background **to)
{
	int ret;
	ret = _hb_jv_parse_bg(from, to);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_collision_flag_and_free(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	int ret;
	ret = _hb_jv_parse_collision_flag(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_collision_flags_and_free(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	int ret;
	ret = _hb_jv_parse_collision_flags(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_trait_and_free(jv from, jv name, struct hb_trait **to)
{
	int ret;
	ret = _hb_jv_parse_trait(from, name, to);
	jv_free(from);
	jv_free(name);
	return ret;
}

static int
_hb_jv_parse_trait_list_and_free(jv from, struct hb_trait ***to)
{
	int ret;
	ret = _hb_jv_parse_trait_list(from, to);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_vertex_and_free(jv from, struct hb_vertex **to,
		struct hb_trait **traits)
{
	int ret;
	ret = _hb_jv_parse_vertex(from, to, traits);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_vertex_list_and_free(jv from, struct hb_vertex ***to,
		struct hb_trait **traits)
{
	int ret;
	ret = _hb_jv_parse_vertex_list(from, to, traits);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_segment_and_free(jv from, struct hb_segment **to,
	   struct hb_vertex **vertexes, struct hb_trait **traits)
{
	int ret;
	ret = _hb_jv_parse_segment(from, to, vertexes, traits);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_segment_list_and_free(jv from, struct hb_segment ***to,
		struct hb_vertex **vertexes, struct hb_trait **traits)
{
	int ret;
	ret = _hb_jv_parse_segment_list(from, to, vertexes, traits);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_vec2_and_free(jv from, float to[2], const float *(fallback[2]))
{
	int ret;
	ret = _hb_jv_parse_vec2(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_team_and_free(jv from, enum hb_team *to, enum hb_team *fallback)
{
	int ret;
	ret = _hb_jv_parse_team(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_goal_and_free(jv from, struct hb_goal **to)
{
	int ret;
	ret = _hb_jv_parse_goal(from, to);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_goal_list_and_free(jv from, struct hb_goal ***to)
{
	int ret;
	ret = _hb_jv_parse_goal_list(from, to);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_ball_physics_and_free(jv from, struct hb_disc **to)
{
	int ret;
	ret = _hb_jv_parse_ball_physics(from, to);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_disc_and_free(jv from, struct hb_disc **to,
		struct hb_trait **traits)
{
	int ret;
	ret = _hb_jv_parse_disc(from, to, traits);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_disc_list_and_free(jv from, struct hb_disc ***to,
		struct hb_trait **traits, struct hb_disc **ball_physics)
{
	int ret;
	ret = _hb_jv_parse_disc_list(from, to, traits, ball_physics);
	jv_free(from);
	return ret;
}

extern struct hb_stadium *
hb_stadium_parse(const char *in)
{
	struct hb_stadium *s;
	jv root;

	/////////////setup
	s = calloc(1, sizeof(struct hb_stadium));
	root = jv_parse(in);

	if (jv_get_kind(root) != JV_KIND_OBJECT)
		goto err;

	/////////////name
	{
		jv name;
		name = jv_object_get(jv_copy(root), jv_string("name"));
		if (_hb_jv_parse_string_and_free(name, &s->name, NULL) < 0)
			goto err;
	}

	/////////////cameraWidth
	{
		jv cam_width;
		cam_width = jv_object_get(jv_copy(root), jv_string("cameraWidth"));
		if (_hb_jv_parse_number_and_free(cam_width, &s->camera_width, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////cameraHeight
	{
		jv cam_height;
		cam_height = jv_object_get(jv_copy(root), jv_string("cameraHeight"));
		if (_hb_jv_parse_number_and_free(cam_height, &s->camera_height, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////maxViewWidth
	{
		jv max_view_width;
		max_view_width = jv_object_get(jv_copy(root), jv_string("maxViewWidth"));
		if (_hb_jv_parse_number_and_free(max_view_width, &s->max_view_width, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////cameraFollow
	{
		jv camera_follow;
		enum hb_camera_follow fallback_camera_follow;
		fallback_camera_follow = HB_CAMERA_FOLLOW_BALL;
		camera_follow = jv_object_get(jv_copy(root), jv_string("cameraFollow"));
		if (_hb_jv_parse_camera_follow_and_free(camera_follow,
					&s->camera_follow, &fallback_camera_follow) < 0)
			goto err;
	}

	/////////////spawnDistance
	{
		jv spawn_distance;
		spawn_distance = jv_object_get(jv_copy(root), jv_string("spawnDistance"));
		if (_hb_jv_parse_number_and_free(spawn_distance, &s->spawn_distance, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////canBeStored
	{
		jv can_be_stored;
		can_be_stored = jv_object_get(jv_copy(root), jv_string("canBeStored"));
		if (_hb_jv_parse_boolean_and_free(can_be_stored, &s->can_be_stored, &HB_B_TRUE) < 0)
			goto err;
	}

	/////////////kickOffReset
	{
		jv kick_off_reset;
		enum hb_kick_off_reset fallback_kick_off_reset;
		fallback_kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
		kick_off_reset = jv_object_get(jv_copy(root), jv_string("kickOffReset"));
		if (_hb_jv_parse_kick_off_reset_and_free(kick_off_reset,
					&s->kick_off_reset, &fallback_kick_off_reset) < 0)
			goto err;
	}

	/////////////bg
	{
		jv bg;
		bg = jv_object_get(jv_copy(root), jv_string("bg"));
		if (_hb_jv_parse_bg_and_free(bg, &s->bg) < 0)
			goto err;
	}

	/////////////traits
	{
		jv traits;
		traits = jv_object_get(jv_copy(root), jv_string("traits"));
		if (_hb_jv_parse_trait_list_and_free(traits, &s->traits) < 0)
			goto err;
	}

	/////////////vertexes
	{
		jv vertexes;
		vertexes = jv_object_get(jv_copy(root), jv_string("vertexes"));
		if (_hb_jv_parse_vertex_list_and_free(vertexes, &s->vertexes, s->traits) < 0)
			goto err;
	}

	/////////////segments
	{
		jv segments;
		segments = jv_object_get(jv_copy(root), jv_string("segments"));
		if (_hb_jv_parse_segment_list_and_free(segments, &s->segments, s->vertexes, s->traits) < 0)
			goto err;
	}

	/////////////goals
	{
		jv goals;
		goals = jv_object_get(jv_copy(root), jv_string("goals"));
		if (_hb_jv_parse_goal_list_and_free(goals, &s->goals) < 0)
			goto err;
	}

	/////////////ballPhysics
	{
		jv ball_physics;
		ball_physics = jv_object_get(jv_copy(root), jv_string("ballPhysics"));
		if (_hb_jv_parse_ball_physics_and_free(ball_physics, &s->ball_physics) < 0)
			goto err;
	}

	/////////////discs
	{
		jv discs;
		discs = jv_object_get(jv_copy(root), jv_string("discs"));
		if (_hb_jv_parse_disc_list_and_free(discs, &s->discs, s->traits, &s->ball_physics) < 0)
			goto err;
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

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
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
	struct hb_trait **trait;
	struct hb_vertex **vertex;
	struct hb_segment **segment;
	struct hb_goal **goal;
	struct hb_disc **disc;
	struct hb_plane **plane;
	struct hb_joint **joint;
	struct hb_point **point;

	if (s->name)
		free(s->name);
	if (s->bg)
		free(s->bg);
	if (s->traits) {
		trait = s->traits;
		while (*trait) {
			free((*trait)->name);
			free(*trait);
			trait++;
		}
		free(s->traits);
	}
	if (s->vertexes) {
		vertex = s->vertexes;
		while (*vertex) {
			free(*vertex);
			vertex++;
		}
		free(s->vertexes);
	}
	if (s->segments) {
		segment = s->segments;
		while (*segment) {
			free(*segment);
			segment++;
		}
		free(s->segments);
	}
	if (s->goals) {
		goal = s->goals;
		while (*goal) {
			free(*goal);
			goal++;
		}
		free(s->goals);
	}
	if (s->discs) {
		if (s->discs[0] != NULL) s->ball_physics = NULL;
		disc = s->discs;
		while (*disc) {
			free(*disc);
			disc++;
		}
		free(s->discs);
	}
	if (s->planes) {
		plane = s->planes;
		while (*plane) {
			free(*plane);
			plane++;
		}
		free(s->planes);
	}
	if (s->joints) {
		joint = s->joints;
		while (*joint) {
			free(*joint);
			joint++;
		}
		free(s->joints);
	}
	if (s->red_spawn_points) {
		point = s->red_spawn_points;
		while (*point) {
			free(*point);
			point++;
		}
		free(s->red_spawn_points);
	}
	if (s->blue_spawn_points) {
		point = s->blue_spawn_points;
		while (*point) {
			free(*point);
			point++;
		}
		free(s->blue_spawn_points);
	}
	if (s->ball_physics) {
		free(s->ball_physics);
	}

	free(s);
}
