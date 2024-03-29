// dt: a simple tool that prints directory tree
// Copyright (C) 2019  Qijian Zhang <qijian.zhang@qq.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>

static void dt_indent(uint32_t n);
static char *dt_namecat(const char *dir, const char *name);
static bool dt_should_skip(const char *name);
static void dt_iter_dir(const char *dir_name, DIR *dp, uint32_t indent);
static int dt_tree_node(const char *dir, const char *name, uint32_t indent);
static int dt_tree(const char *dir_name);

static void dt_indent(uint32_t n)
{
	if (n == 0) {
		return;
	}
	n -= 1;
	while (n) {
		printf("|  ");
		n -= 1;
	}
	printf("|- ");
}

static char *dt_namecat(const char *dir, const char *name)
{
	size_t dir_len = strlen(dir);
	size_t name_len = strlen(name);
	size_t len = dir_len + name_len + 1;
	char *r = malloc(len + 1);
	strcpy(r, dir);
	r[dir_len] = '/';
	strcpy(r + dir_len + 1, name);
	r[len] = 0;
	return r;
}

static bool dt_should_skip(const char *name)
{
	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
		return true;
	}
	return false;
}

static void dt_iter_dir(const char *dir_name, DIR *dp, uint32_t indent)
{
	struct dirent *e = NULL;
	while ((e = readdir(dp)) != NULL) {
		if (dt_should_skip(e->d_name)) {
			continue;
		}
		if (e->d_type == DT_DIR) {
			dt_tree_node(dir_name, e->d_name, indent);
		} else {
			dt_indent(indent);
			puts(e->d_name);
		}
	}
}

static int dt_tree_node(const char *dir, const char *name, uint32_t indent)
{
	char *dir_name = dt_namecat(dir, name);
	DIR *dp = opendir(dir_name);
	if (dp == NULL) {
		return 1;
	}

	dt_indent(indent);
	puts(name);

	indent += 1;
	dt_iter_dir(dir_name, dp, indent);

	free(dir_name);
	closedir(dp);
	return 0;
}

static int dt_tree(const char *dir_name)
{
	size_t dlen = strlen(dir_name);
	char *dir = malloc(dlen + 1);
	strcpy(dir, dir_name);

	while (dlen > 1) {
		dlen -= 1;
		if (dir[dlen] == '/') {
			dir[dlen] = 0;
		} else {
			break;
		}
	}

	DIR *dp = opendir(dir);
	if (dp == NULL) {
		fprintf(stderr, "Failed to open dir: %s\n", dir_name);
		return 1;
	}

	puts(dir);
	dt_iter_dir(dir_name, dp, 1);

	free(dir);
	closedir(dp);
	return 0;
}

int main(int argc, char *argv[])
{
	const char *name = ".";
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			name = argv[i];
		} else {
			printf("dt -- print directory tree\n"
			       "Usage: dt [dir] [--help]\n"
			       "current directory is used if dir is omitted\n");
			return 0;
		}
	}

	return dt_tree(name);
}

