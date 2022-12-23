/*

	cc -std=c99 -pedantic -Wall -Wextra -Os -I/usr/include/cairo -s \
			-o stadium_to_svg stadium_to_svg.c -lcairo -ljq -lhb -lm

	TODO:
		stadium:segments:curve
		stadium:bg:type:grass
		stadium:bg:type:hockey
		stadium:bg:cornerRadius
		stadium:planes
		stadium:joints

*/

#include <cairo/cairo-svg.h>
#include <cairo/cairo.h>
#include <hb/stadium.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void
cairo_set_source_rgb_uint32(cairo_t *cr, uint32_t color)
{
	double r, g, b;

	r = (double)((color >> 16) & 0xff) / 255;
	g = (double)((color >>  8) & 0xff) / 255;
	b = (double)((color >>  0) & 0xff) / 255;

    cairo_set_source_rgb (cr, r, g, b);
}

static void
cairo_render_stadium_bg(cairo_t *cr, struct hb_stadium *s)
{
	cairo_surface_t *image;
	cairo_pattern_t *pattern;
	switch (s->bg->type) {
	case HB_BACKGROUND_TYPE_HOCKEY:
		image = cairo_image_surface_create_from_png ("assets/concrete2.png");
		pattern = cairo_pattern_create_for_surface (image);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source (cr, pattern);
		cairo_rectangle (cr, -s->width, -s->height, s->width*2, s->height*2);
		cairo_fill (cr);
		cairo_surface_destroy(image);
		cairo_pattern_destroy(pattern);
		image = cairo_image_surface_create_from_png ("assets/concrete.png");
		pattern = cairo_pattern_create_for_surface (image);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source (cr, pattern);
		cairo_rectangle (cr, -s->bg->width, -s->bg->height, s->bg->width*2, s->bg->height*2);
		cairo_fill (cr);
		cairo_surface_destroy(image);
		cairo_pattern_destroy(pattern);
		break;
	case HB_BACKGROUND_TYPE_GRASS:
		cairo_set_source_rgb_uint32(cr, s->bg->color);
		cairo_rectangle (cr, -s->width, -s->height, s->width*2, s->height*2);
		cairo_fill(cr);
		image = cairo_image_surface_create_from_png ("assets/grass.png");
		pattern = cairo_pattern_create_for_surface (image);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source (cr, pattern);
		cairo_rectangle (cr, -s->bg->width, -s->bg->height, s->bg->width*2, s->bg->height*2);
		cairo_fill (cr);
		cairo_surface_destroy(image);
		cairo_pattern_destroy(pattern);
		break;
	case HB_BACKGROUND_TYPE_NONE:
		cairo_set_line_width(cr, 3);
		cairo_set_source_rgb_uint32(cr, s->bg->color);
		cairo_rectangle(cr, -s->width, -s->height, s->width*2, s->height*2);
		cairo_fill(cr);
		break;
	}
}

static void
cairo_render_stadium_segments(cairo_t *cr, struct hb_stadium *s)
{
	struct hb_vertex *v0, *v1;
	cairo_set_line_width(cr, 3);
	hb_stadium_segments_foreach(s, segment) {
		if (!segment->vis || segment->curve != 0)
			continue;
		v0 = s->vertexes[segment->v0];
		v1 = s->vertexes[segment->v1];
		cairo_set_source_rgb_uint32(cr, segment->color);
		cairo_move_to(cr, v0->x, v0->y);
		cairo_line_to(cr, v1->x, v1->y);
		cairo_stroke(cr);
	}
}

static void
cairo_render_stadium_discs(cairo_t *cr, struct hb_stadium *s)
{
	hb_stadium_discs_foreach(s, disc) {
		cairo_set_source_rgb_uint32(cr, disc->color);
		cairo_arc(cr, disc->pos[0], disc->pos[1], disc->radius, 0, M_PI*2);
		cairo_fill(cr);
		cairo_set_source_rgb_uint32(cr, 0);
		cairo_set_line_width(cr, 2);
		cairo_arc(cr, disc->pos[0], disc->pos[1], disc->radius, 0, M_PI*2);
		cairo_stroke(cr);
		cairo_set_line_width(cr, 3);
	}
}

int
main(int argc, char **argv)
{
	struct hb_stadium *stadium;
	const char *stadium_path, *out_svg_path;
	cairo_surface_t *surface;
	cairo_t *cr;

	stadium_path = argc > 1 ? argv[1] : "stadium.hbs";
	out_svg_path = argc > 2 ? argv[2] : "stadium.svg";

	if (NULL == (stadium = hb_stadium_from_file(stadium_path))) {
		fprintf(stderr, "stadium_to_svg: couldn't load %s\n",
				stadium_path);
		exit(1);
	}

	surface = cairo_svg_surface_create(out_svg_path, stadium->width * 2,
			stadium->height * 2);

	cr = cairo_create(surface);

	cairo_translate(cr, stadium->width, stadium->height);
	cairo_render_stadium_bg(cr, stadium);
	cairo_render_stadium_segments(cr, stadium);
	cairo_render_stadium_discs(cr, stadium);
	cairo_translate(cr, -stadium->width, -stadium->height);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	return 0;
}
