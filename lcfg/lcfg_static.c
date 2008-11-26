/* This file is an autogenerated single-file version of liblcfg.
 * The revision used to create this file is 997. It is recommended
 * that you update this file on a regulary basis from the original
 * liblcfg distribution package.
 *
 * The most recent version of liblcfg is available at
 *   <http://liblcfg.mwcollect.org>
 */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "lcfg_static.h"
/*** begin file include/lcfg/lcfg_string.h ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_string.h 995 2007-05-07 01:49:42Z dp $
 * 
 */

#ifndef LCFG_STRING_H
#define LCFG_STRING_H

struct lcfg_string *  lcfg_string_new();
struct lcfg_string *  lcfg_string_new_copy(struct lcfg_string *);
int                   lcfg_string_set(struct lcfg_string *, const char *);
int                   lcfg_string_cat_char(struct lcfg_string *, char);
int                   lcfg_string_cat_cstr(struct lcfg_string *, const char *);
int                   lcfg_string_cat_uint(struct lcfg_string *, unsigned int);
int                   lcfg_string_find(struct lcfg_string *, char);
int                   lcfg_string_rfind(struct lcfg_string *, char);
void                  lcfg_string_trunc(struct lcfg_string *, unsigned int);
inline const char *   lcfg_string_cstr(struct lcfg_string *);
inline unsigned int   lcfg_string_len(struct lcfg_string *);
void                  lcfg_string_delete(struct lcfg_string *);

#endif
/*** end file include/lcfg/lcfg_string.h ***/
/*** begin file include/lcfg/lcfg_token.h ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_token.h 995 2007-05-07 01:49:42Z dp $
 * 
 */

#ifndef LCFG_TOKEN_H
#define LCFG_TOKEN_H


enum lcfg_token_type {
	lcfg_null_token = 0,
	lcfg_identifier,
	lcfg_equals,
	lcfg_string,
	lcfg_sbracket_open,
	lcfg_sbracket_close,
	lcfg_comma,
	lcfg_brace_open,
	lcfg_brace_close
};

extern const char *lcfg_token_map[];

struct lcfg_token
{
	enum lcfg_token_type type;
	struct lcfg_string *string;
	short line;
	short col;
};


#endif
/*** end file include/lcfg/lcfg_token.h ***/
/*** begin file include/lcfg/lcfg_scanner.h ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_scanner.h 995 2007-05-07 01:49:42Z dp $
 * 
 */

#ifndef LCFG_SCANNER_H
#define LCFG_SCANNER_H


struct lcfg_scanner;
struct lcfg_token;

struct lcfg_scanner *    lcfg_scanner_new(struct lcfg *, int fd);
enum lcfg_status         lcfg_scanner_init(struct lcfg_scanner *);
enum lcfg_status         lcfg_scanner_next_token(struct lcfg_scanner *, struct lcfg_token *);
int                      lcfg_scanner_has_next(struct lcfg_scanner *);
void                     lcfg_scanner_delete(struct lcfg_scanner *);

#endif
/*** end file include/lcfg/lcfg_scanner.h ***/
/*** begin file include/lcfg/lcfg_parser.h ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_parser.h 995 2007-05-07 01:49:42Z dp $
 * 
 */

#ifndef LCFG_PARSER_H
#define LCFG_PARSER_H


struct lcfg_parser;

struct lcfg_parser *  lcfg_parser_new(struct lcfg *, const char *);
enum lcfg_status      lcfg_parser_run(struct lcfg_parser *);
enum lcfg_status      lcfg_parser_accept(struct lcfg_parser *, lcfg_visitor_function, void *);
void                  lcfg_parser_delete(struct lcfg_parser *);
enum lcfg_status      lcfg_parser_get(struct lcfg_parser *, const char *, void **, size_t *);

#endif
/*** end file include/lcfg/lcfg_parser.h ***/
/*** begin file src/lcfg_string.c ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_string.c 995 2007-05-07 01:49:42Z dp $
 * 
 */



struct lcfg_string
{
	char *str;
	unsigned int size;
	unsigned int capacity;
};

int lcfg_string_set(struct lcfg_string *s, const char *cstr)
{
	lcfg_string_trunc(s, 0);
	return lcfg_string_cat_cstr(s, cstr);
}


/* make sure new_size bytes fit into the string */
inline static void lcfg_string_grow(struct lcfg_string *s, unsigned int new_size)
{
	/* always allocate one byte more than needed
	 * to make _cstr() working in any case without realloc. */
	while( (new_size + 1) > s->capacity )
	{
		s->capacity *= 2;
		s->str = realloc(s->str, s->capacity);
	}
}

struct lcfg_string *lcfg_string_new()
{
	struct lcfg_string *s = malloc(sizeof(struct lcfg_string));
	assert(s);
	
	s->capacity = 8;
	s->size = 0;
	s->str = malloc(s->capacity);
	
	assert(s->str);
	
	return s;
}

struct lcfg_string *lcfg_string_new_copy(struct lcfg_string *s)
{
	struct lcfg_string *s_new = malloc(sizeof(struct lcfg_string));
	assert(s_new);
	
	s_new->capacity = s->capacity;
	s_new->size = s->size;
	s_new->str = malloc(s_new->capacity);
	
	memcpy(s_new->str, s->str, s_new->size);
	
	return s_new;
}

int lcfg_string_cat_uint(struct lcfg_string *s, unsigned int i)
{
	unsigned int size_needed = 1;
	unsigned int ii = i;
	char c;
	
	while( ii > 10 )
	{
		size_needed++;
		ii /= 10;
	}
	
	lcfg_string_grow(s, s->size + size_needed);
	
	ii = size_needed - 1;
	do
	{
		c = '0' + i % 10;
		s->str[s->size + ii--] = c;
		i /= 10;
	} while( i != 0 );
	
	s->size += size_needed;
	
	return s->size;
}

int lcfg_string_find(struct lcfg_string *s, char c)
{
	int i;
	
	for( i = 0; i < s->size; i++ )
	{
		if( s->str[i] == c )
		{
			return i;
		}
	}
	
	return -1;
}

int lcfg_string_rfind(struct lcfg_string *s, char c)
{
	int i;
	
	for( i = s->size - 1; i >= 0; i-- )
	{
		if( s->str[i] == c )
		{
			return i;
		}
	}
	
	return -1;
}

void lcfg_string_trunc(struct lcfg_string *s, unsigned int max_size)
{
	if( max_size < s->size )
	{
		s->size = max_size;
	}
}

int lcfg_string_cat_cstr(struct lcfg_string *s, const char *cstr)
{
	size_t len = strlen(cstr);
	
	lcfg_string_grow(s, s->size + len);
	
	memcpy(s->str + s->size, cstr, len);
	
	s->size += len;
	
	return s->size;
}

int lcfg_string_cat_char(struct lcfg_string *s, char c)
{
	lcfg_string_grow(s, s->size + 1);
	
	s->str[s->size++] = c;
	
	return s->size;
}

inline const char *lcfg_string_cstr(struct lcfg_string *s)
{
	s->str[s->size] = '\0';
	return s->str;
}

inline unsigned int lcfg_string_len(struct lcfg_string *s)
{
	return s->size;
}

void lcfg_string_delete(struct lcfg_string *s)
{
	free(s->str);
	free(s);
}
/*** end file src/lcfg_string.c ***/
/*** begin file src/lcfg_token.c ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_token.c 997 2007-05-07 02:11:53Z dp $
 * 
 */


const char *lcfg_token_map[] = {
	"null_token",
	"T_IDENTIFIER",
	"`='",
	"T_STRING",
	"`['",
	"`]'",
	"`,'",
	"`{'",
	"`}'"
 };
/*** end file src/lcfg_token.c ***/
/*** begin file src/lcfg_scanner.c ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_scanner.c 995 2007-05-07 01:49:42Z dp $
 * 
 */



#define BUFFER_SIZE 0xff

struct lcfg_scanner
{
	struct lcfg *lcfg;
	
	int fd;
	char buffer[BUFFER_SIZE];
	int offset;
	int size;
	int eof;
	
	short line;
	short col;
	
	struct lcfg_token prepared_token;
	int token_eof;
};


static enum lcfg_status lcfg_scanner_buffer_fill(struct lcfg_scanner *s)
{
	if( (s->size = read(s->fd, s->buffer, BUFFER_SIZE)) < 0 )
	{
		lcfg_error_set(s->lcfg, "read(): %s", strerror(errno));
		return lcfg_status_error;
	}
	else if( s->size == 0 )
	{
		s->eof = !0;
	}
	else
	{
		s->offset = 0;
	}
	
	return lcfg_status_ok;
}

static inline int lcfg_scanner_char_eof(struct lcfg_scanner *s)
{
	if( s->eof )
	{
		return !0;
	}
	else
	{
		if( s->size == 0 || s->offset == BUFFER_SIZE )
		{
			lcfg_scanner_buffer_fill(s);
		}
		if( s->size < BUFFER_SIZE && s->offset == s->size )
		{
			s->eof = !0;
		}
	
		return s->eof;
	}
}

static enum lcfg_status lcfg_scanner_char_read(struct lcfg_scanner *s, char *c)
{
	if( lcfg_scanner_char_eof(s) )
	{
		lcfg_error_set(s->lcfg, "%s", "cannot read beyond eof");
		return lcfg_status_error;
	}
	
	*c = s->buffer[s->offset++];

	return lcfg_status_ok;
}

static enum lcfg_status lcfg_scanner_char_peek(struct lcfg_scanner *s, char *c)
{
	if( lcfg_scanner_char_eof(s) )
	{
		lcfg_error_set(s->lcfg, "%s", "cannot peek beyond eof");
		return lcfg_status_error;
	}
	
	*c = s->buffer[s->offset];

	return lcfg_status_ok;
}

/* the beautiful lowlevel fsm */
static enum lcfg_status lcfg_scanner_token_read(struct lcfg_scanner *s)
{
	enum scanner_state { start = 0, comm_start, in_oneline, in_multiline, multiline_end, in_identifier, in_str, in_esc, esc_hex_exp_first, esc_hex_exp_second, invalid };
	enum scanner_state state = start;
	char c = '\0';
	char hex[3];
	
	s->prepared_token.type = lcfg_null_token;
	
	while( !lcfg_scanner_char_eof(s) )
	{
		int consume = !0;
		lcfg_scanner_char_peek(s, &c);
		
		switch( state )
		{
			case start:
				switch( c )
				{
					case ' ':
					case '\t':
					case '\r':
					case '\n':
						break;
					case '=':
						s->prepared_token.type = lcfg_equals;
						break;
					case '[':
						s->prepared_token.type = lcfg_sbracket_open;
						break;
					case ']':
						s->prepared_token.type = lcfg_sbracket_close;
						break;
					case '{':
						s->prepared_token.type = lcfg_brace_open;
						break;
					case '}':
						s->prepared_token.type = lcfg_brace_close;
						break;
					case ',':
						s->prepared_token.type = lcfg_comma;
						break;
					case '/':
						state = comm_start;
						break;
					case '"':
						state = in_str;
						lcfg_string_trunc(s->prepared_token.string, 0);
						break;
					default:
						if( isalpha(c) )
						{
							lcfg_string_trunc(s->prepared_token.string, 0);
							lcfg_string_cat_char(s->prepared_token.string, c);
							state = in_identifier;
						}
						else
						{
							lcfg_error_set(s->lcfg, "parse error: invalid input character `%c' (0x%02x) near line %d, col %d", isprint(c) ? c : '.', c, s->line, s->col);
							state = invalid;
						}
				}
				break;
			case comm_start:
				if( c == '/' )
				{
					state = in_oneline;
				}
				else if( c == '*' )
				{
					state = in_multiline;
				}
				else
				{
					lcfg_error_set(s->lcfg, "parse error: invalid input character `%c' (0x%02x) near line %d, col %d", isprint(c) ? c : '.', c, s->line, s->col);
					state = invalid;
				}
				break;
			case in_oneline:
				if( c == '\n' )
				{
					state = start;
				}
				break;
			case in_multiline:
				if( c == '*' )
				{
					state = multiline_end;
				}
				break;
			case multiline_end:
				if( c == '/' )
				{
					state = start;
				}
				else if( c != '*' )
				{
					state = in_multiline;
				}
				break;
			case in_identifier:
				if( isalnum(c) || c == '-' || c == '_' )
				{
					lcfg_string_cat_char(s->prepared_token.string, c);
				}
				else
				{
					s->prepared_token.type = lcfg_identifier;
					consume = 0;
					state = start;
				}
				break;
			case in_str:
				if( c == '"' )
				{
					s->prepared_token.type = lcfg_string;
					state = start;
				}
				else if( c == '\\' )
				{
					state = in_esc;
				}
				else
				{
					lcfg_string_cat_char(s->prepared_token.string, c);
				}
				break;
			case in_esc:
				state = in_str;
				switch( c )
				{
					case '"':
						lcfg_string_cat_char(s->prepared_token.string, '"');
						break;
					case 'n':
						lcfg_string_cat_char(s->prepared_token.string, '\n');
						break;
					case 't':
						lcfg_string_cat_char(s->prepared_token.string, '\t');
						break;
					case 'r':
						lcfg_string_cat_char(s->prepared_token.string, '\r');
						break;
					case '0':
						lcfg_string_cat_char(s->prepared_token.string, '\0');
						break;
					case 'x':
						state = esc_hex_exp_first;
						break;
					default:
						lcfg_error_set(s->lcfg, "invalid string escape sequence `%c' near line %d, col %d", c, s->line, s->col);
						state = invalid;
				}
				break;
			case esc_hex_exp_first:
				if( !isxdigit(c) )
				{
					lcfg_error_set(s->lcfg, "invalid hex escape sequence `%c' on line %d column %d", c, s->line, s->col);
					state = invalid;
				}
				hex[0] = c;
				state = esc_hex_exp_second;
				break;
			case esc_hex_exp_second:
				if( !isxdigit(c) )
				{
					lcfg_error_set(s->lcfg, "invalid hex escape sequence `%c' on line %d column %d", c, s->line, s->col);
					state = invalid;
				}
				hex[1] = c;
				hex[2] = '\0';
				lcfg_string_cat_char(s->prepared_token.string, strtoul(hex, NULL, 16));
				state = in_str;
				break;
			case invalid:
				break;
		}
		/*#include <stdio.h>
		printf("read %c at line %d column %d, new state is %d\n", isprint(c) ? c : '.', s->line, s->col, state);*/
		
		/* this is technically not optimal (token position identified by last char), but it will suffice for now */
		s->prepared_token.line = s->line;
		s->prepared_token.col = s->col;
	
		if( consume )
		{
			lcfg_scanner_char_read(s, &c);
			if( c == '\n' )
			{
				s->line++;
				s->col = 1;
			}
			else
			{
				s->col++;
			}
		}
		
		if( s->prepared_token.type != lcfg_null_token || state == invalid )
		{
			break;
		}
	}
	
	if( state != start )
	{
		if( state != invalid )
		{
			lcfg_error_set(s->lcfg, "parse error: premature end of file near line %d, col %d", s->line, s->col);
		}
		
		return lcfg_status_error;
	}
	
	return lcfg_status_ok;
}

enum lcfg_status lcfg_scanner_init(struct lcfg_scanner *s)
{
	/* prepare the first token */
	return lcfg_scanner_token_read(s); 
}

int lcfg_scanner_has_next(struct lcfg_scanner *s)
{
	return s->prepared_token.type != lcfg_null_token;
}

enum lcfg_status lcfg_scanner_next_token(struct lcfg_scanner *s, struct lcfg_token *t)
{
	if( !lcfg_scanner_has_next(s) )
	{
		lcfg_error_set(s->lcfg, "%s", "cannot access tokenstream beyond eof");
		return lcfg_status_error;
	}
	
	memcpy(t, &s->prepared_token, sizeof(struct lcfg_token));
	t->string = lcfg_string_new_copy(s->prepared_token.string);
	
	/* prepare the next token */
	return lcfg_scanner_token_read(s); 
}

struct lcfg_scanner *lcfg_scanner_new(struct lcfg *c, int fd)
{
	struct lcfg_scanner *s = malloc(sizeof(struct lcfg_scanner));
	assert(s);
	
	memset(s, 0, sizeof(struct lcfg_scanner));
	
	s->lcfg = c;
	s->fd = fd;
	
	s->line = s->col = 1;
	
	s->prepared_token.string = lcfg_string_new();
	
	return s;
}

void lcfg_scanner_delete(struct lcfg_scanner *s)
{
	lcfg_string_delete(s->prepared_token.string);
	free(s);
}

/*** end file src/lcfg_scanner.c ***/
/*** begin file src/lcfg_parser.c ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg_parser.c 995 2007-05-07 01:49:42Z dp $
 * 
 */



#ifndef strdup
char *strdup(const char *s)
{
	char *sdup;
	
	size_t len = strlen(s) + 1;
	sdup = malloc(len);

	if( sdup != NULL )
	{
		memcpy(sdup, s, len);
	}
	
	return sdup;
}
#endif

struct lcfg_parser_value_pair
{
	char *key;
	struct lcfg_string *value;
};


struct lcfg_parser
{
	struct lcfg *lcfg;
	char *filename;
	struct lcfg_scanner *scanner;
	
	struct lcfg_parser_value_pair *values;
	unsigned int value_length;
	unsigned int value_capacity;
};

static int lcfg_parser_add_value(struct lcfg_parser *p, const char *key, struct lcfg_string *value)
{
	if( p->value_length == p->value_capacity )
	{
		p->value_capacity *= 2;
		p->values = realloc(p->values, sizeof(struct lcfg_parser_value_pair) * p->value_capacity);
		assert(p->values);
	}
	
	p->values[p->value_length].key = strdup(key);
	p->values[p->value_length].value = lcfg_string_new_copy(value);
	
	return ++p->value_length;
}

struct lcfg_parser *lcfg_parser_new(struct lcfg *c, const char *filename)
{
	struct lcfg_parser *p = malloc(sizeof(struct lcfg_parser));
	assert(p);
	
	memset(p, 0, sizeof(struct lcfg_parser));
	
	p->filename = strdup(filename);
	p->lcfg = c;
	
	p->value_length = 0;
	p->value_capacity = 8;
	p->values = malloc(sizeof(struct lcfg_parser_value_pair) * p->value_capacity);
	assert(p->values);
	
	return p;
}

/* this is a basic push down automata */
static enum lcfg_status lcfg_parser_parse(struct lcfg_parser *p)
{
	enum state { top_level = 0, exp_equals, exp_value, in_list, in_map, invalid };
	/*const char *state_map[] = { "top_level", "exp_equals", "exp_value", "in_list", "in_map", "invalid" };*/

	struct state_element
	{
		enum state s;
		int list_counter;
	};

	/* start of ugly preproc stuff */
	#define STATE_STACK_PUSH(t) \
	if( ssi + 1 == state_stack_size ) \
	{ \
		state_stack_size *= 2; \
		state_stack = realloc(state_stack, state_stack_size * sizeof(struct state_element)); \
	} \
	state_stack[++ssi].s = t; \
	state_stack[ssi].list_counter = 0
	#define STATE_STACK_POP() ssi--
	#define PATH_PUSH_STR(s) \
	if( lcfg_string_len(current_path) != 0 ) \
	{ \
		lcfg_string_cat_char(current_path, '.'); \
	} \
	lcfg_string_cat_cstr(current_path, s);
	#define PATH_PUSH_INT(i) \
	if( lcfg_string_len(current_path) != 0 ) \
	{ \
		lcfg_string_cat_char(current_path, '.'); \
	} \
	lcfg_string_cat_uint(current_path, i);
	#define PATH_POP() \
	if( lcfg_string_rfind(current_path, '.') != -1 ) \
	{ \
		lcfg_string_trunc(current_path, lcfg_string_rfind(current_path, '.')); \
	} \
	else \
	{ \
		lcfg_string_trunc(current_path, 0); \
	}
	/* end of ugly preproc stuff */

	if( lcfg_scanner_init(p->scanner) != lcfg_status_ok )
	{
		return lcfg_status_error;
	}
	
	int state_stack_size = 8;
	int ssi = 0; /* ssi = state stack index */
	struct state_element *state_stack = malloc(sizeof(struct state_element) * state_stack_size);
	
	state_stack[ssi].s = top_level;
	state_stack[ssi].list_counter = 0;
	
	struct lcfg_token t;
	struct lcfg_string *current_path = lcfg_string_new();
	
	while( lcfg_scanner_has_next(p->scanner) && state_stack[ssi].s != invalid )
	{
		if( lcfg_scanner_next_token(p->scanner, &t) != lcfg_status_ok )
		{
			free(state_stack);
			lcfg_string_delete(t.string);
			lcfg_string_delete(current_path);
			return lcfg_status_error;
		}
		
		switch( state_stack[ssi].s )
		{
			case top_level:
			case in_map:
				if( t.type == lcfg_identifier )
				{
					PATH_PUSH_STR(lcfg_string_cstr(t.string));
					STATE_STACK_PUSH(exp_equals);
				}
				else if( state_stack[ssi].s == in_map && t.type == lcfg_brace_close )
				{
					STATE_STACK_POP();
					PATH_POP();
				}
				else
				{
					lcfg_error_set(p->lcfg, "invalid token (%s) near line %d column %d: expected identifier%s", lcfg_token_map[t.type], t.line, t.col, state_stack[ssi].s == in_map ? " or `}'" : ""); 
					state_stack[ssi].s = invalid;
				}
				break;
			case exp_equals:
				if( t.type == lcfg_equals )
					state_stack[ssi].s = exp_value;
				else
				{
					lcfg_error_set(p->lcfg, "invalid token (%s) near line %d column %d: expected `='", lcfg_token_map[t.type], t.line, t.col); 
					state_stack[ssi].s = invalid;
				}
				break;
			case exp_value:
				if( t.type == lcfg_string )
				{
					lcfg_parser_add_value(p, lcfg_string_cstr(current_path), t.string);
					/*printf("adding string value for single statement\n");*/
					STATE_STACK_POP();
					PATH_POP();
				}
				else if( t.type == lcfg_sbracket_open )
				{
					state_stack[ssi].s = in_list;
				}
				else if( t.type == lcfg_brace_open )
				{
					state_stack[ssi].s = in_map;
				}
				else
				{
					lcfg_error_set(p->lcfg, "invalid token (%s) near line %d column %d: expected string, `[' or `{'", lcfg_token_map[t.type], t.line, t.col); 
					state_stack[ssi].s = invalid;
				}
				break;
			case in_list:
				if( t.type == lcfg_comma ); /* ignore comma */
				else if( t.type == lcfg_string )
				{
					PATH_PUSH_INT(state_stack[ssi].list_counter);
					lcfg_parser_add_value(p, lcfg_string_cstr(current_path), t.string);
					PATH_POP();
					/*printf("adding string to list pos %d\n", state_stack[ssi].list_counter);*/
					state_stack[ssi].list_counter++;
				}
				else if( t.type == lcfg_sbracket_open )
				{
					PATH_PUSH_INT(state_stack[ssi].list_counter);
					/*printf("adding list to list pos %d\n", state_stack[ssi].list_counter);*/
					state_stack[ssi].list_counter++;
					STATE_STACK_PUSH(in_list);
				}
				else if( t.type == lcfg_brace_open )
				{
					PATH_PUSH_INT(state_stack[ssi].list_counter);
					/*printf("adding map to list pos %d\n", state_stack[ssi].list_counter);*/
					state_stack[ssi].list_counter++;
					STATE_STACK_PUSH(in_map);
				}
				else if( t.type == lcfg_sbracket_close )
				{
					PATH_POP();
					STATE_STACK_POP();
				}
				else
				{
					lcfg_error_set(p->lcfg, "invalid token (%s) near line %d column %d: expected string, `[', `{', `,' or `]'", lcfg_token_map[t.type], t.line, t.col); 
					state_stack[ssi].s = invalid;
				}
				break;
			case invalid: /* unreachable */
				assert(0);
				break;
		}
		
		lcfg_string_delete(t.string);
		
		/*printf(" *** pda: read %s, state is now %s\n", lcfg_token_map[t.type], state_map[state_stack[ssi].s]);*/
	}

	lcfg_string_delete(current_path);	
	
	if( state_stack[ssi].s == top_level && ssi == 0 )
	{
		free(state_stack);
		return lcfg_status_ok;
	}
	else
	{
		free(state_stack);
		return lcfg_status_error;
	}
}

enum lcfg_status lcfg_parser_run(struct lcfg_parser *p)
{	
	int fd = open(p->filename, 0);
	enum lcfg_status status;
	
	if( fd < 0 )
	{
		lcfg_error_set(p->lcfg, "open(): %s", strerror(errno));
		return lcfg_status_error;
	}
	
	p->scanner = lcfg_scanner_new(p->lcfg, fd);
	
	status = lcfg_parser_parse(p);	
	
	close(fd);
	
	return status;
}
enum lcfg_status lcfg_parser_accept(struct lcfg_parser *p, lcfg_visitor_function fn, void *user_data)
{
	int i;
	
	for( i = 0; i < p->value_length; i++ )
	{
		if( fn(p->values[i].key, (void *)lcfg_string_cstr(p->values[i].value), lcfg_string_len(p->values[i].value), user_data) != lcfg_status_ok )
		{
			lcfg_error_set(p->lcfg, "%s", "configuration value traversal aborted upon user request");
			return lcfg_status_error;
		}
	}
	
	return lcfg_status_ok;
}

enum lcfg_status lcfg_parser_get(struct lcfg_parser *p, const char *key, void **data, size_t *len)
{
	int i;

	for( i = 0; i < p->value_length; i++ )
	{
		if( !strcmp(p->values[i].key, key) )
		{
			*data = (void *)lcfg_string_cstr(p->values[i].value);
			*len = lcfg_string_len(p->values[i].value);
			return lcfg_status_ok;
		}
	}
	
	return lcfg_status_error;
}


void lcfg_parser_delete(struct lcfg_parser *p)
{
	if( p->scanner != NULL )
	{
		lcfg_scanner_delete(p->scanner);
	}
	
	int i;
	
	for( i = 0; i < p->value_length; i++ )
	{
		free(p->values[i].key);
		lcfg_string_delete(p->values[i].value);
	}
	free(p->values);
	free(p->filename);
	free(p);
}
/*** end file src/lcfg_parser.c ***/
/*** begin file src/lcfg.c ***/
/*
 * liblcfg - lightweight configuration file library
 * Copyright (C) 2007  Paul Baecher
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    $Id: lcfg.c 995 2007-05-07 01:49:42Z dp $
 * 
 */



struct lcfg
{
	char error[0xff];
	struct lcfg_parser *parser;
};

struct lcfg *lcfg_new(const char *filename)
{
	struct lcfg *c = malloc(sizeof(struct lcfg));
	assert(c);
	memset(c, 0, sizeof(struct lcfg));
	
	c->parser = lcfg_parser_new(c, filename);
	assert(c->parser);
	
	return c;
}

void lcfg_delete(struct lcfg *c)
{
	lcfg_parser_delete(c->parser);
	free(c);
}

const char *lcfg_error_get(struct lcfg *c)
{
	return c->error;
}

enum lcfg_status lcfg_parse(struct lcfg *c)
{
	return lcfg_parser_run(c->parser);
}

enum lcfg_status lcfg_accept(struct lcfg *c, lcfg_visitor_function fn, void *user_data)
{
	return lcfg_parser_accept(c->parser, fn, user_data);
}

enum lcfg_status lcfg_value_get(struct lcfg *c, const char *key, void **data, size_t *len)
{
	return lcfg_parser_get(c->parser, key, data, len);
}

void lcfg_error_set(struct lcfg *c, const char *fmt, ...)
{	
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(c->error, sizeof(c->error), fmt, ap);
	va_end(ap);
}

/*** end file src/lcfg.c ***/
