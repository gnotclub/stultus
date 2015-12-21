/* See LICENSE file for copyright and license details. */
#include "pkg.h"

static int pkg_remove_cb(struct db *, struct pkg *, void *);

static void
usage(void)
{
	fprintf(stderr, VERSION " (c) 2014 morpheus engineers\n");
	fprintf(stderr, "usage: %s [-v] [-f] [-r path] pkg...\n", argv0);
	fprintf(stderr, "  -v    Enable verbose output\n");
	fprintf(stderr, "  -f    Force the removal of empty directories and symlinks\n");
	fprintf(stderr, "  -r    Set alternative installation root\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	struct db *db;
	char *root = "/";
	int i, r;

	ARGBEGIN {
	case 'v':
		vflag = 1;
		break;
	case 'f':
		fflag = 1;
		break;
	case 'r':
		root = ARGF();
		break;
	default:
		usage();
	} ARGEND;

	if (argc < 1)
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
		r = db_walk(db, pkg_remove_cb, argv[i]);
		if (r < 0) {
			db_free(db);
			exit(EXIT_FAILURE);
		} else if (r == 0) {
			printf("%s is not installed\n", argv[i]);
		}
	}

	db_free(db);

	return EXIT_SUCCESS;
}

static int
pkg_remove_cb(struct db *db, struct pkg *pkg, void *name)
{
	if (strcmp(pkg->name, name) == 0) {
		if (pkg_remove(db, pkg) < 0)
			return -1;
		if (db_rm(db, pkg) < 0)
			return -1;
		printf("removed %s\n", pkg->name);
		return 1;
	}
	return 0;
}
