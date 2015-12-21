/* See LICENSE file for copyright and license details. */
#include "pkg.h"

static void
usage(void)
{
	fprintf(stderr, VERSION " (c) 2014 morpheus engineers\n");
	fprintf(stderr, "usage: %s [-v] [-f] [-r path] pkg...\n", argv0);
	fprintf(stderr, "  -v    Enable verbose output\n");
	fprintf(stderr, "  -f    Override filesystem checks and force installation\n");
	fprintf(stderr, "  -r    Set alternative installation root\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	struct db *db;
	struct pkg *pkg;
	char path[PATH_MAX];
	char *root = "/";
	int i;

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
	if (db_load(db) < 0) {
		db_free(db);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < argc; i++) {
		if (!realpath(argv[i], path)) {
			weprintf("realpath %s:", argv[i]);
			db_free(db);
			exit(EXIT_FAILURE);
		}
		if (vflag == 1)
			printf("installing %s\n", path);
		pkg = pkg_load_file(db, path);
		if (!pkg) {
			db_free(db);
			exit(EXIT_FAILURE);
		}
		if (fflag == 0) {
			if (pkg_collisions(pkg) < 0) {
				printf("not installed %s\n", path);
				db_free(db);
				exit(EXIT_FAILURE);
			}
		}
		if (db_add(db, pkg) < 0) {
			db_free(db);
			exit(EXIT_FAILURE);
		}
		if (pkg_install(db, pkg) < 0) {
			db_free(db);
			exit(EXIT_FAILURE);
		}
		printf("installed %s\n", path);
	}

	db_free(db);
	return EXIT_SUCCESS;
}
