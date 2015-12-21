/* See LICENSE file for copyright and license details. */
#include "pkg.h"

/* Extract the package name from a filename.  e.g. /tmp/pkg#version.pkg.tgz */
void
parse_name(const char *path, char **name)
{
	char tmp[PATH_MAX], filename[PATH_MAX], *p;

	estrlcpy(tmp, path, sizeof(tmp));
	estrlcpy(filename, basename(tmp), sizeof(filename));
	/* strip extension */
	p = strrchr(filename, '.');
	if (!p)
		goto err;
	*p = '\0';
	p = strrchr(filename, '.');
	if (!p)
		goto err;
	*p = '\0';
	/* extract name */
	p = strchr(filename, '#');
	if (p)
		*p = '\0';
	if (filename[0] == '\0')
		goto err;
	*name = estrdup(filename);
	return;
err:
	eprintf("%s: invalid package filename\n",
		path);
}

/* Extract the package version from a filename.  e.g. /tmp/pkg#version.pkg.tgz */
void
parse_version(const char *path, char **version)
{
	char tmp[PATH_MAX], filename[PATH_MAX], *p;

	estrlcpy(tmp, path, sizeof(tmp));
	estrlcpy(filename, basename(tmp), sizeof(filename));
	/* strip extension */
	p = strrchr(filename, '.');
	if (!p)
		goto err;
	*p = '\0';
	p = strrchr(filename, '.');
	if (!p)
		goto err;
	*p = '\0';
	/* extract version */
	p = strchr(filename, '#');
	if (!p) {
		*version = NULL;
		return;
	}
	p++;
	if (*p == '\0')
		goto err;
	*version = estrdup(p);
	return ;
err:
	eprintf("%s: invalid package filename\n",
		path);
}

void
parse_db_name(const char *path, char **name)
{
	char tmp[PATH_MAX], *p;

	estrlcpy(tmp, path, sizeof(tmp));
	p = strchr(tmp, '#');
	if (p)
		*p = '\0';
	*name = estrdup(tmp);
}

void
parse_db_version(const char *path, char **version)
{
	char tmp[PATH_MAX], *p;

	estrlcpy(tmp, path, sizeof(tmp));
	p = strchr(tmp, '#');
	if (p)
		*p = '\0';
	if (p)
		*version = estrdup(p + 1);
	else
		*version = NULL;
}
