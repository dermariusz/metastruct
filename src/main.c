// Copyright (c) 2023 Mariusz Wojcik
// This file is part of the metastruct project and is licensed under the MIT license.
// For more information see LICENSE.md

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <metastruct.h>

typedef struct person_s {
	char *name;
	int age;
} person_t;

METASTRUCT_DEFINE(person,
	METASTRUCT_MEMBER(person_t, name),
	METASTRUCT_MEMBER(person_t, age)
);

int main(int argc, char *argv[]) {

	person_t p = {"Mariusz", 25};

	person_metastruct_print(&p, "Hello my name is {name} and I am {age} years old.\n");

	return 0;
}
