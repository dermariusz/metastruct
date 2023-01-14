// Copyright (c) 2023 Mariusz Wojcik
// This file is part of the metastruct project is licensed under the MIT license.
// For more information see LICENSE.md

#pragma once

#include <stdio.h>
#include <stddef.h>

#define METASTRUCT_DEFINE(_prefix, ...) \
	static const metastruct_meta_t _prefix##_metastruct [] = { \
		__VA_ARGS__, \
		{NULL, 0, 0, -1} \
	}; \
	int _prefix##_metastruct_print(void *ptr, const char *format) { metastruct_print(_prefix, ptr, format); } \
	int _prefix##_metastruct_fprint(void *ptr, const char *format, FILE *out) { metastruct_fprint(_prefix, ptr, format, out); }



#define METASTRUCT_MEMBER(_struct, _name) \
		{#_name, sizeof(#_name)-1, offsetof(_struct, _name), sizeof ((_struct *) NULL)->_name, METASTRUCT_GENHINT(_struct, _name)}

#define METASTRUCT_GENHINT(_struct, _name) \
			_Generic(((_struct *) NULL)->_name, \
				double: 'd', \
				float: 'f', \
				int: 'i', \
				default: 's' \
			)


typedef struct metastruct_meta_s {
	const char * name;
	size_t namelen;
	int offs;
	int len;
	char type_hint;
} metastruct_meta_t;

#define metastruct_print(_prefix, _ptr, _format) metastruct_fmt_internal_fprint( _prefix##_metastruct, _ptr, _format, stdout)
#define metastruct_fprint(_prefix, _ptr, _format, _out) metastruct_fmt_internal_fprint(_prefix##_metastruct, _ptr, _format, _out)

int metastruct_fmt_internal_fprint(const metastruct_meta_t * meta, void *ptr, const char *format, FILE * out);