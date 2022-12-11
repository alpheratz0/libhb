#include <hb/stadium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jv.h>

static long
_get_file_size(FILE *fp)
{
	long   current_pos;
	long     file_size;

	current_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, current_pos, SEEK_SET);

	return file_size;
}

static void
_err_unmatched_property_type(const char *prop_name,
                             jv_kind expected, jv_kind got)
{
	fprintf(stderr, "hb_stadium: unmatched type "
		"for property \"%s\" expected %s got %s\n",
		prop_name, jv_kind_name(expected), jv_kind_name(got));
}

static void
_err_unmatched_property_value(const char *prop_name,
                              const char *valid_values, const char *got_value)
{
	fprintf(stderr, "hb_stadium: unmatched value "
		"for property \"%s\" expected %s got %s\n",
		prop_name, valid_values, got_value);
}

static int
_hb_trait_parse(jv root, struct hb_trait *trait)
{
	/////////////curve
	{
		jv             curve;
		jv_kind   curve_kind;

		curve = jv_object_get(jv_copy(root), jv_string("curve"));
		curve_kind = jv_get_kind(curve);

		if (curve_kind == JV_KIND_NUMBER) {
			trait->has_curve = true;
			trait->curve = jv_number_value(curve);
		} else if (curve_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.curve",
					JV_KIND_NUMBER, curve_kind);
			return -1;
		}
	}

	/////////////damping
	{
		jv             damping;
		jv_kind   damping_kind;

		damping = jv_object_get(jv_copy(root), jv_string("damping"));
		damping_kind = jv_get_kind(damping);

		if (damping_kind == JV_KIND_NUMBER) {
			trait->has_damping = true;
			trait->damping = jv_number_value(damping);
		} else if (damping_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.damping",
					JV_KIND_NUMBER, damping_kind);
			return -1;
		}
	}

	/////////////invMass
	{
		jv             inv_mass;
		jv_kind   inv_mass_kind;

		inv_mass = jv_object_get(jv_copy(root), jv_string("invMass"));
		inv_mass_kind = jv_get_kind(inv_mass);

		if (inv_mass_kind == JV_KIND_NUMBER) {
			trait->has_inv_mass = true;
			trait->inv_mass = jv_number_value(inv_mass);
		} else if (inv_mass_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.invMass",
					JV_KIND_NUMBER, inv_mass_kind);
			return -1;
		}
	}

	/////////////radius
	{
		jv             radius;
		jv_kind   radius_kind;

		radius = jv_object_get(jv_copy(root), jv_string("radius"));
		radius_kind = jv_get_kind(radius);

		if (radius_kind == JV_KIND_NUMBER) {
			trait->has_radius = true;
			trait->radius = jv_number_value(radius);
		} else if (radius_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.radius",
					JV_KIND_NUMBER, radius_kind);
			return -1;
		}
	}

	/////////////bCoef
	{
		jv             b_coef;
		jv_kind   b_coef_kind;

		b_coef = jv_object_get(jv_copy(root), jv_string("bCoef"));
		b_coef_kind = jv_get_kind(b_coef);

		if (b_coef_kind == JV_KIND_NUMBER) {
			trait->has_b_coef = true;
			trait->b_coef = jv_number_value(b_coef);
		} else if (b_coef_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.bCoef",
					JV_KIND_NUMBER, b_coef_kind);
			return -1;
		}
	}

	/////////////color
	{
		jv                  color;
		jv_kind        color_kind;
		const char     *color_str;
		char           *parse_end;
		jv                r, g, b;
		jv_kind            r_kind;
		jv_kind            g_kind;
		jv_kind            b_kind;
		int               arr_len;

		color = jv_object_get(jv_copy(root), jv_string("color"));
		color_kind = jv_get_kind(color);

		if (color_kind == JV_KIND_STRING) {
			color_str = jv_string_value(color);

			if (!strcmp(color_str, "transparent")) {
				trait->has_color = true;
				trait->color = 0x00000000;
			} else {
				trait->color = strtol(color_str, &parse_end, 16);

				if (parse_end - color_str != 6 ||
						parse_end[1] != '\0') {
					_err_unmatched_property_value("trait.color",
							"RRGGBB", color_str);
					return -1;
				}

				trait->has_color = true;
			}
		} else if (color_kind == JV_KIND_ARRAY) {
			if (jv_array_length(color) == 3) {
				r = jv_array_get(color, 0);
				g = jv_array_get(color, 1);
				b = jv_array_get(color, 2);

				r_kind = jv_get_kind(r);
				g_kind = jv_get_kind(g);
				b_kind = jv_get_kind(b);

				if (r_kind == JV_KIND_NUMBER &&
						g_kind == JV_KIND_NUMBER &&
						b_kind == JV_KIND_NUMBER) {
					trait->has_color = true;
					trait->color =
											 (0xff << 24) |
						((int)(jv_number_value(r)) << 16) |
						((int)(jv_number_value(g)) <<  8) |
						((int)(jv_number_value(b)) <<  0);

				} else {
					// FIXME: convert the array to string in an error func
					_err_unmatched_property_value("trait.color",
							"[R, G, B]", "unknown");
					return -1;
				}
			} else {
				// FIXME: convert the array to string in an error func
				_err_unmatched_property_value("trait.color",
						"[R, G, B]", "unknown");
				return -1;
			}
		} else if (color_kind != JV_KIND_INVALID) {
			_err_unmatched_property_value("trait.color",
					"[R, G, B]", "unknown");
			return -1;
		}
	}

	/////////////vis
	{
		jv             vis;
		jv_kind   vis_kind;

		vis = jv_object_get(jv_copy(root), jv_string("vis"));
		vis_kind = jv_get_kind(vis);

		if (vis_kind == JV_KIND_TRUE ||
				vis_kind == JV_KIND_FALSE) {
			trait->has_vis = true;
			trait->vis = vis_kind == JV_KIND_TRUE;
		} else if (vis_kind != JV_KIND_INVALID) {
			_err_unmatched_property_value("trait.vis",
					"[true, false]", jv_kind_name(vis_kind));
			return -1;
		}
	}

	/////////////cGroup
	{
		// TODO: not implemented
	}

	/////////////cMask
	{
		// TODO: not implemented
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

	/////////////name
	{
		jv            name;
		jv_kind  name_kind;

		name = jv_object_get(jv_copy(root), jv_string("name"));
		name_kind = jv_get_kind(name);

		if (name_kind == JV_KIND_STRING) {
			s->name = strdup(jv_string_value(name));
		} else {
			_err_unmatched_property_type("name", JV_KIND_STRING, name_kind);
			goto err;
		}
	}

	/////////////cameraWidth & cameraHeight
	{
		jv             camera_width;
		jv            camera_height;
		jv_kind   camera_width_kind;
		jv_kind  camera_height_kind;

		camera_width = jv_object_get(jv_copy(root), jv_string("cameraWidth"));
		camera_height = jv_object_get(jv_copy(root), jv_string("cameraHeight"));

		camera_width_kind = jv_get_kind(camera_width);
		camera_height_kind = jv_get_kind(camera_height);

		if (camera_width_kind == JV_KIND_NUMBER &&
				camera_height_kind == JV_KIND_NUMBER) {
			s->camera_width = jv_number_value(camera_width);
			s->camera_height = jv_number_value(camera_height);
		} else if (camera_width_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("cameraWidth",
					JV_KIND_NUMBER, camera_width_kind);
			goto err;
		} else if (camera_height_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("cameraHeight",
					JV_KIND_NUMBER, camera_height_kind);
			goto err;
		} else {
			s->camera_width = s->camera_height = 0.0f;
		}
	}

	/////////////maxViewWidth
	{
		jv             max_view_width;
		jv_kind   max_view_width_kind;

		max_view_width = jv_object_get(jv_copy(root), jv_string("maxViewWidth"));
		max_view_width_kind = jv_get_kind(max_view_width);

		if (max_view_width_kind == JV_KIND_NUMBER) {
			s->max_view_width = jv_number_value(max_view_width);
		} else if (max_view_width_kind == JV_KIND_INVALID) {
			s->max_view_width = 0.0f;
		} else {
			_err_unmatched_property_type("maxViewWidth",
					JV_KIND_NUMBER, max_view_width_kind);
			goto err;
		}
	}

	/////////////cameraFollow
	{
		jv                  camera_follow;
		jv_kind        camera_follow_kind;
		const char     *camera_follow_str;

		camera_follow = jv_object_get(jv_copy(root), jv_string("cameraFollow"));
		camera_follow_kind = jv_get_kind(camera_follow);

		if (camera_follow_kind == JV_KIND_STRING) {
			camera_follow_str = jv_string_value(camera_follow);
			if (!strcmp(camera_follow_str, "ball")) {
				s->camera_follow = HB_CAMERA_FOLLOW_BALL;
			} else if (!strcmp(camera_follow_str, "player")) {
				s->camera_follow = HB_CAMERA_FOLLOW_PLAYER;
			} else {
				_err_unmatched_property_value("cameraFollow",
						"[ball, player]", camera_follow_str);
				goto err;
			}
		} else if (camera_follow_kind == JV_KIND_INVALID) {
			s->camera_follow = HB_CAMERA_FOLLOW_BALL;
		} else {
			_err_unmatched_property_type("cameraFollow",
					JV_KIND_STRING, camera_follow_kind);
			goto err;
		}
	}

	/////////////spawnDistance
	{
		jv                  spawn_distance;
		jv_kind        spawn_distance_kind;

		spawn_distance = jv_object_get(jv_copy(root), jv_string("spawnDistance"));
		spawn_distance_kind = jv_get_kind(spawn_distance);

		if (spawn_distance_kind == JV_KIND_NUMBER) {
			s->spawn_distance = jv_number_value(spawn_distance);
		} else if (spawn_distance_kind == JV_KIND_INVALID) {
			s->spawn_distance = 0.0f;
		} else {
			_err_unmatched_property_type("spawnDistance",
					JV_KIND_NUMBER, spawn_distance_kind);
			goto err;
		}
	}

	/////////////canBeStored
	{
		jv                  can_be_stored;
		jv_kind        can_be_stored_kind;

		can_be_stored = jv_object_get(jv_copy(root), jv_string("canBeStored"));
		can_be_stored_kind = jv_get_kind(can_be_stored);

		if (can_be_stored_kind == JV_KIND_TRUE ||
				can_be_stored_kind == JV_KIND_FALSE) {
			s->can_be_stored = can_be_stored_kind == JV_KIND_TRUE;
		} else if (can_be_stored_kind == JV_KIND_INVALID) {
			s->can_be_stored = true;
		} else {
			_err_unmatched_property_value("canBeStored",
					"[true, false]", jv_kind_name(can_be_stored_kind));
			goto err;
		}
	}

	/////////////kickOffReset
	{
		jv                  kick_off_reset;
		jv_kind        kick_off_reset_kind;
		const char     *kick_off_reset_str;

		kick_off_reset = jv_object_get(jv_copy(root), jv_string("kickOffReset"));
		kick_off_reset_kind = jv_get_kind(kick_off_reset);

		if (kick_off_reset_kind == JV_KIND_STRING) {
			kick_off_reset_str = jv_string_value(kick_off_reset);
			if (!strcmp(kick_off_reset_str, "partial")) {
				s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
			} else if (!strcmp(kick_off_reset_str, "full")) {
				s->kick_off_reset = HB_KICK_OFF_RESET_FULL;
			} else {
				_err_unmatched_property_value("kickOffReset",
						"[partial, full]", kick_off_reset_str);
				goto err;
			}
		} else if (kick_off_reset_kind == JV_KIND_INVALID) {
			s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
		} else {
			_err_unmatched_property_type("kickOffReset",
					JV_KIND_STRING, kick_off_reset_kind);
			goto err;
		}
	}

	/////////////bg
	{
		jv                  bg;
		jv_kind        bg_kind;

		s->bg = malloc(sizeof(struct hb_background));

		bg = jv_object_get(jv_copy(root), jv_string("bg"));
		bg_kind = jv_get_kind(bg);

		if (bg_kind == JV_KIND_OBJECT) {
			/////////////type
			{
				jv                  type;
				jv_kind        type_kind;
				const char     *type_str;

				type = jv_object_get(jv_copy(bg), jv_string("type"));
				type_kind = jv_get_kind(type);

				if (type_kind == JV_KIND_STRING) {
					type_str = jv_string_value(type);
					if (!strcmp(type_str, "none")) {
						s->bg->type = HB_BACKGROUND_TYPE_NONE;
					} else if (!strcmp(type_str, "grass")) {
						s->bg->type = HB_BACKGROUND_TYPE_GRASS;
					} else if (!strcmp(type_str, "hockey")) {
						s->bg->type = HB_BACKGROUND_TYPE_HOCKEY;
					} else {
						_err_unmatched_property_value("bg.type",
								"[none, grass, hockey]", type_str);
						goto err;
					}
				} else if (type_kind == JV_KIND_INVALID) {
					s->bg->type = HB_BACKGROUND_TYPE_NONE;
				} else {
					_err_unmatched_property_type("bg.type",
							JV_KIND_STRING, type_kind);
					goto err;
				}
			}

			/////////////width
			{
				jv                  width;
				jv_kind        width_kind;

				width = jv_object_get(jv_copy(bg), jv_string("width"));
				width_kind = jv_get_kind(width);

				if (width_kind == JV_KIND_NUMBER) {
					s->bg->width = jv_number_value(width);
				} else if (width_kind == JV_KIND_INVALID) {
					s->bg->width = 0.0f;
				} else {
					_err_unmatched_property_type("bg.width",
							JV_KIND_NUMBER, width_kind);
					goto err;
				}
			}

			/////////////height
			{
				jv                  height;
				jv_kind        height_kind;

				height = jv_object_get(jv_copy(bg), jv_string("height"));
				height_kind = jv_get_kind(height);

				if (height_kind == JV_KIND_NUMBER) {
					s->bg->height = jv_number_value(height);
				} else if (height_kind == JV_KIND_INVALID) {
					s->bg->height = 0.0f;
				} else {
					_err_unmatched_property_type("bg.height",
							JV_KIND_NUMBER, height_kind);
					goto err;
				}
			}

			/////////////kickOffRadius
			{
				jv                  kick_off_radius;
				jv_kind        kick_off_radius_kind;

				kick_off_radius = jv_object_get(jv_copy(bg), jv_string("kickOffRadius"));
				kick_off_radius_kind = jv_get_kind(kick_off_radius);

				if (kick_off_radius_kind == JV_KIND_NUMBER) {
					s->bg->kick_off_radius = jv_number_value(kick_off_radius);
				} else if (kick_off_radius_kind == JV_KIND_INVALID) {
					s->bg->kick_off_radius = 0.0f;
				} else {
					_err_unmatched_property_type("bg.kickOffRadius",
							JV_KIND_NUMBER, kick_off_radius_kind);
					goto err;
				}
			}

			/////////////cornerRadius
			{
				jv                  corner_radius;
				jv_kind        corner_radius_kind;

				corner_radius = jv_object_get(jv_copy(bg), jv_string("cornerRadius"));
				corner_radius_kind = jv_get_kind(corner_radius);

				if (corner_radius_kind == JV_KIND_NUMBER) {
					s->bg->corner_radius = jv_number_value(corner_radius);
				} else if (corner_radius_kind == JV_KIND_INVALID) {
					s->bg->corner_radius = 0.0f;
				} else {
					_err_unmatched_property_type("bg.cornerRadius",
							JV_KIND_NUMBER, corner_radius_kind);
					goto err;
				}
			}

			/////////////goalLine
			{
				jv                  goal_line;
				jv_kind        goal_line_kind;

				goal_line = jv_object_get(jv_copy(bg), jv_string("goalLine"));
				goal_line_kind = jv_get_kind(goal_line);

				if (goal_line_kind == JV_KIND_NUMBER) {
					s->bg->goal_line = jv_number_value(goal_line);
				} else if (goal_line_kind == JV_KIND_INVALID) {
					s->bg->goal_line = 0.0f;
				} else {
					_err_unmatched_property_type("bg.goalLine",
							JV_KIND_NUMBER, goal_line_kind);
					goto err;
				}
			}

			/////////////color
			{
				jv                  color;
				jv_kind        color_kind;
				const char     *color_str;
				char           *parse_end;
				jv                r, g, b;
				jv_kind            r_kind;
				jv_kind            g_kind;
				jv_kind            b_kind;
				int               arr_len;

				color = jv_object_get(jv_copy(bg), jv_string("color"));
				color_kind = jv_get_kind(color);

				if (color_kind == JV_KIND_STRING) {
					color_str = jv_string_value(color);

					if (!strcmp(color_str, "transparent")) {
						s->bg->color = 0x00000000;
					} else {
						s->bg->color = strtol(color_str, &parse_end, 16);

						if (parse_end - color_str != 6 ||
								parse_end[1] != '\0') {
							_err_unmatched_property_value("bg.color",
									"RRGGBB", color_str);
							goto err;
						}
					}
				} else if (color_kind == JV_KIND_INVALID) {
					s->bg->color = 0xff718c5a;
				} else if (color_kind == JV_KIND_ARRAY) {
					if (jv_array_length(color) == 3) {
						r = jv_array_get(color, 0);
						g = jv_array_get(color, 1);
						b = jv_array_get(color, 2);

						r_kind = jv_get_kind(r);
						g_kind = jv_get_kind(g);
						b_kind = jv_get_kind(b);

						if (r_kind == JV_KIND_NUMBER &&
								g_kind == JV_KIND_NUMBER &&
								b_kind == JV_KIND_NUMBER) {
							s->bg->color =
								                     (0xff << 24) |
								((int)(jv_number_value(r)) << 16) |
								((int)(jv_number_value(g)) <<  8) |
								((int)(jv_number_value(b)) <<  0);

						} else {
							// FIXME: convert the array to string in an error func
							_err_unmatched_property_value("bg.color",
									"[R, G, B]", "unknown");
							goto err;
						}
					} else {
						// FIXME: convert the array to string in an error func
						_err_unmatched_property_value("bg.color",
								"[R, G, B]", "unknown");
						goto err;
					}
				} else {
					_err_unmatched_property_type("bg.color",
							JV_KIND_STRING, color_kind);
					goto err;
				}
			}
		} else if (bg_kind == JV_KIND_INVALID) {
			s->bg->type = HB_BACKGROUND_TYPE_NONE;
			s->bg->width = 0.0f;
			s->bg->height = 0.0f;
			s->bg->kick_off_radius = 0.0f;
			s->bg->corner_radius = 0.0f;
			s->bg->goal_line = 0.0f;
			s->bg->color = 0xff718c5a;
		} else {
			_err_unmatched_property_type("bg",
					JV_KIND_OBJECT, bg_kind);
			goto err;
		}
	}

	/////////////traits
	{
		jv                  traits;
		jv_kind        traits_kind;
		int             traits_len;
		int            trait_index;

		traits = jv_object_get(jv_copy(root), jv_string("traits"));
		traits_kind = jv_get_kind(traits);

		if (traits_kind == JV_KIND_OBJECT) {
			trait_index = 0;
			traits_len = jv_object_length(jv_copy(traits));
			s->traits = calloc(traits_len + 1, sizeof(struct hb_trait *));

			jv_object_foreach(traits, key, value) {
				s->traits[trait_index] = calloc(1, sizeof(struct hb_trait));
				s->traits[trait_index]->name = strdup(jv_string_value(key));
				if (_hb_trait_parse(value, s->traits[trait_index++]) < 0) {
					goto err;
				}
			}
		} else if (traits_kind == JV_KIND_INVALID) {
			s->traits = calloc(1, sizeof(struct hb_trait *));
		} else {
			_err_unmatched_property_type("traits",
					JV_KIND_OBJECT, traits_kind);
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
	FILE                         *fp;
	long                   file_size;
	char                   *raw_data;
	struct hb_stadium             *s;

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
	/* if (t->has_c_group) printf("Trait::%s.cGroup: %.2f\n", t->name, t->c_group); */
	/* if (t->has_c_mask) printf("Trait::%s.cMask: %.2f\n", t->name, t->c_mask); */
}

extern const char *
hb_stadium_print(struct hb_stadium *s)
{
	struct hb_trait **trait;

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
