/* See LICENSE file for copyright and license details. */
#include "pkg.h"

static int own_pkg_cb(struct db *, struct pkg *, void *);

static void
usage(void)
{
	fprintf(stderr, VERSION " (c) 2014 morpheus engineers\n");
	fprintf(stderr, "usage: %s [-r path] [-o filename...]\n", argv0);
	fprintf(stderr, "  -r	 Set alternative installation root\n");
	fprintf(stderr, "  -o	 Look for the packages that own the given filename(s)\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	struct db *db;
	char path[PATH_MAX];
	char *root = "/";
	int oflag = 0;
	int i, r;

	ARGBEGIN {
	case 'o':
		oflag = 1;
		break;
	case 'r':
		root = ARGF();
		break;
	default:
		usage();
	} ARGEND;

	if (oflag == 0 || argc < 1)
		usage();

	db = db_new(root);
	if (!db)
		exit(EXIT_FAILURE);
	r = db_load(db);
	if (r < 0) {
		db_free(db);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < argc; i++) {
		if (!realpath(argv[i], path)) {
			weprintf("realpath %s:", argv[i]);
			db_free(db);
			exit(EXIT_FAILURE);
		}
		r = db_walk(db, own_pkg_cb, path);
		if (r < 0) {
			db_free(db);
			exit(EXIT_FAILURE);
		}
	}

	db_free(db);

	return EXIT_SUCCESS;
}

static int
own_pkg_cb(struct db *db, struct pkg *pkg, void *file)
{
	char *path = file;
	struct pkgentry *pe;
	struct stat sb1, sb2;

	(void) db;

	if (lstat(path, &sb1) < 0)
		eprintf("lstat %s:", path);

	TAILQ_FOREACH(pe, &pkg->pe_head, entry) {
		if (lstat(pe->path, &sb2) < 0) {
			weprintf("lstat %s:", pe->path);
			continue;
		}
		if (sb1.st_dev == sb2.st_dev &&
		    sb1.st_ino == sb2.st_ino) {
			printf("%s is owned by %s\n", path, pkg->name);
			break;
		}
	}
	return 0;
}
