/* See LICENSE file for copyright and license details. */
#include <archive.h>
#include <archive_entry.h>
#include <dirent.h>
#include <ftw.h>
#include <limits.h>
#include <regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include "arg.h"
#include "queue.h"

#define LEN(x) (sizeof (x) / sizeof *(x))

#define DBPATH        "/var/pkg"
#define DBPATHREJECT  "/etc/pkgtools/reject.conf"
#define ARCHIVEBUFSIZ BUFSIZ

struct pkgentry {
	char path[PATH_MAX];		/* absolute path of package entry */
	char rpath[PATH_MAX];		/* relative path of package entry */
	TAILQ_ENTRY(pkgentry) entry;
};

struct pkg {
	char *name;			/* package name */
	char *version;			/* package version */
	char path[PATH_MAX];		/* path to package in db or .pkg.tgz */
	TAILQ_HEAD(pe_head, pkgentry) pe_head;
	TAILQ_ENTRY(pkg) entry;
};

struct rejrule {
	regex_t preg;
	TAILQ_ENTRY(rejrule) entry;
};

struct db {
	DIR *pkgdir;			/* opendir() handle for DBPATH */
	char root[PATH_MAX];		/* db root to allow for installation in a mountpoint */
	char path[PATH_MAX];		/* absolute path to DBPATH including db root */
	TAILQ_HEAD(rejrule_head, rejrule) rejrule_head;
	TAILQ_HEAD(pkg_head, pkg) pkg_head;
	TAILQ_HEAD(pkg_rm_head, pkg) pkg_rm_head;
};

/* db.c */
extern int fflag;
extern int vflag;

/* eprintf.c */
extern char *argv0;

/* common.c */
void parse_db_name(const char *, char **);
void parse_db_version(const char *, char **);
void parse_name(const char *, char **);
void parse_version(const char *, char **);

/* db.c */
struct db *db_new(const char *);
int db_free(struct db *);
int db_add(struct db *, struct pkg *);
int db_rm(struct db *, struct pkg *);
int db_load(struct db *);
struct pkg *pkg_load_file(struct db *, const char *);
int db_walk(struct db *, int (*)(struct db *, struct pkg *, void *), void *);
int db_links(struct db *, const char *);

/* ealloc.c */
void *ecalloc(size_t, size_t);
void *emalloc(size_t size);
void *erealloc(void *, size_t);
char *estrdup(const char *);

/* eprintf.c */
void enprintf(int, const char *, ...);
void eprintf(const char *, ...);
void weprintf(const char *, ...);

/* pkg.c */
struct pkg *pkg_load(struct db *, const char *);
int pkg_install(struct db *, struct pkg *);
int pkg_remove(struct db *, struct pkg *);
int pkg_collisions(struct pkg *);
struct pkg *pkg_new(const char *, const char *, const char *);
void pkg_free(struct pkg *);
struct pkgentry *pkgentry_new(struct db *, const char *);
void pkgentry_free(struct pkgentry *);

/* reject.c */
void rej_free(struct db *);
int rej_load(struct db *);
int rej_match(struct db *, const char *);

/* strlcat.c */
#undef strlcat
size_t strlcat(char *, const char *, size_t);
size_t estrlcat(char *, const char *, size_t);

/* strlcpy.c */
#undef strlcpy
size_t strlcpy(char *, const char *, size_t);
size_t estrlcpy(char *, const char *, size_t);
