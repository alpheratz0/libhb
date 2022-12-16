/*
 * Copyright 2017 Ryan Armstrong <ryan@cavaliercoder.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * C String Builder - https://github.com/cavaliercoder/c-stringbuilder
 *
 * sb.c is a simple, non-thread safe String Builder that makes use of a
 * dynamically-allocated linked-list to enable linear time appending and
 * concatenation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "sb.h"

struct sb_string_fragment {
	struct sb_string_fragment *next;
	int length;
	char *str;
};

struct sb_string_builder {
	struct sb_string_fragment *root;
	struct sb_string_fragment *trunk;
	int length;
};

/*
 * sb_create returns a pointer to a new struct sb_string_builder
 * or NULL if memory is not available.
 */
extern struct sb_string_builder *
sb_create(void)
{
	struct sb_string_builder *sb;
	sb = calloc(1, sizeof(struct sb_string_builder));
	return sb;
}

/*
 * sb_empty returns non-zero if the given struct sb_string_builder is empty.
 */
extern int
sb_empty(struct sb_string_builder *sb)
{
	return (sb->root == NULL);
}

/*
 * sb_append adds a copy of the given string to a struct sb_string_builder.
 */
extern int
sb_append(struct sb_string_builder *sb, const char *str)
{
	int length;
	struct sb_string_fragment *frag;

	if (NULL == str || '\0' == *str)
		return sb->length;

	length = strlen(str);
	frag = malloc(sizeof(struct sb_string_fragment) + length);

	if (NULL == frag)
		return SB_FAILURE;

	frag->next = NULL;
	frag->length = length;

	memcpy((void*) &frag->str, (const void*) str, length + 1);
	sb->length += length;

	if (NULL == sb->root) sb->root = frag;
	else sb->trunk->next = frag;

	sb->trunk = frag;

	return sb->length;
}

/*
 * sb_appendf adds a copy of the given formatted string
 * to a struct sb_string_builder.
 */
extern int
sb_appendf(struct sb_string_builder *sb, const char *format, ...)
{
	int rc;
	char buf[SB_MAX_FRAG_LENGTH];
	va_list args;

	rc = 0;
	va_start(args, format);
	rc = vsnprintf(&buf[0], SB_MAX_FRAG_LENGTH, format, args);
	va_end(args);

	if (0 > rc)
		return SB_FAILURE;

	return sb_append(sb, buf);
}

/*
 * sb_concat returns a concatenation of strings that have
 * been appended to the struct sb_string_builder. It is the callers
 * responsibility to free the returned reference.
 *
 * The struct sb_string_builder is not modified by this function and
 * can therefore continue to be used.
 */
extern char *
sb_concat(struct sb_string_builder *sb)
{
	char *buf, *c;
	struct sb_string_fragment *frag;

	c = NULL;
	frag = NULL;
	buf = malloc(sb->length + 1);

	if (NULL == buf)
		return NULL;

	c = buf;

	for (frag = sb->root; frag; frag = frag->next) {
		memcpy(c, &frag->str, frag->length);
		c += frag->length;
	}

	*c = '\0';

	return buf;
}

/*
 * sb_reset resets the given struct sb_string_builder,
 * freeing all previously appended strings.
 */
extern void
sb_reset(struct sb_string_builder *sb)
{
	struct sb_string_fragment *frag, *next;

	frag = sb->root;
	next = NULL;

	while (frag) {
		next = frag->next;
		free(frag);
		frag = next;
	}

	sb->root = NULL;
	sb->trunk = NULL;
	sb->length = 0;
}

/*
 * sb_free frees the given struct sb_string_builder and all
 * of its appended strings.
 */
extern void
sb_free(struct sb_string_builder *sb)
{
	sb_reset(sb);
	free(sb);
}
