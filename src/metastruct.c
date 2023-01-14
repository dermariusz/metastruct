// Copyright (c) 2023 Mariusz Wojcik
// This file is part of the metastruct project is licensed under the MIT license.
// For more information see LICENSE.md

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#include "metastruct.h"

enum metastruct_fmt_type_e {
	METASTRUCT_FMT_NONE,
	METASTRUCT_FMT_VAR,
	METASTRUCT_FMT_TEXT
};

typedef struct metastruct_fmt_token_s {
	enum metastruct_fmt_type_e type;
	size_t begin;
	size_t end;
} metastruct_fmt_token_t;

static inline bool metastruct_fmt_next_token(metastruct_fmt_token_t * restrict tok, const char * str, int * pos)
{
	int begin, end;
	tok->type = METASTRUCT_FMT_NONE;

	for (end = begin = *pos; str[end] && !tok->type; end += 1)

		if (str[end + 1] == '\0' || str[end + 1] == '{')
			tok->type = METASTRUCT_FMT_TEXT;

		else if (str[end] == '}' && !(end > 1 && str[end-1] == '}'))
			tok->type = METASTRUCT_FMT_VAR;

	tok->begin = begin;
	tok->end = end;
	*pos = end;
	return tok->type != METASTRUCT_FMT_NONE;
}

int metastruct_fmt_internal_fprint(const metastruct_meta_t * meta, void *ptr, const char *format, FILE * out)
{
	metastruct_fmt_token_t tok;
	int pos = 0;
	int ret_code = 0;

	while (metastruct_fmt_next_token(&tok, format, &pos))
	{
		if (tok.type == METASTRUCT_FMT_TEXT)
		{
			fwrite(format + tok.begin, sizeof(char), tok.end - tok.begin, out);
			continue;
		}

		const char * colon = memchr(format + tok.begin, ':', tok.end - tok.begin);
		int var_begin = tok.begin + 1;
		int var_end = colon ? colon - format: tok.end - 1;

		unsigned i, found;

		// Naive search in meta data of struct for token variable
		for (i = 0, found = false; meta[i].name && !found; i += !found)
			// found is set true when lengths of metadata name and token variable match and when contents of metadata name and token variable match
			found = meta[i].namelen == var_end - var_begin && memcmp(format + var_begin, meta[i].name, meta[i].namelen) == 0;

		if (!found) {
			fputs("{ metastruct member not found: ", out);
			fwrite(format + var_begin, sizeof(char), var_end - var_begin, out);
			fputs(" }", out);
			ret_code = 1;
			continue;
		}

		union {
			void *    target;
			uint8_t * offs;
			char **   strp;
			char *    str;
			double *  dnum;
			float  *  fnum;
			int64_t * inum;
		} rslt;

		rslt.target = ptr;
		rslt.offs += meta[i].offs;

		char format_buf[12] = {'%', '\0'};

		if (colon) {
			assert(tok.end - var_end - 2 < 10 && "Formatbuffer too small");
			strncat(format_buf, format + var_end + 1, tok.end - var_end - 2);
		}
		else switch(meta[i].type_hint) {
			case 'd':
				strcat(format_buf, "lf");
				break;

			case 'f':
				strcat(format_buf, "f");
				break;

			case 'i':
				strcat(format_buf, "d");
				break;

			case 's':
			default:
				strcat(format_buf, "s");
				break;
		}

		switch(meta[i].type_hint) {
			case 'd':
				fprintf(out, format_buf, *rslt.dnum);
				break;

			case 'f':
				fprintf(out, format_buf, *rslt.fnum);
				break;

			case 'i':
				fprintf(out, format_buf, *rslt.inum);
				break;

			case 's':
			default:
				fprintf(out, format_buf, *rslt.strp);
		}
	}
	return ret_code;
}