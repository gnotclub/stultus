/* See LICENSE file for copyright and license details. */
#include "pkg.h"

void
rej_free(struct db *db)
{
	struct rejrule *rule, *tmp;

	for (rule = TAILQ_FIRST(&db->rejrule_head); rule; rule = tmp) {
		tmp = TAILQ_NEXT(rule, entry);
		regfree(&rule->preg);
		free(rule);
	}
}

/* Parse reject.conf and pre-compute regexes */
int
rej_load(struct db *db)
{
	struct rejrule *rule;
	char rejpath[PATH_MAX];
	FILE *fp;
	char *buf = NULL;
	size_t sz = 0;
	ssize_t len;
	int r;

	estrlcpy(rejpath, db->root, sizeof(rejpath));
	estrlcat(rejpath, DBPATHREJECT, sizeof(rejpath));

	if (!(fp = fopen(rejpath, "r")))
		return -1;

	while ((len = getline(&buf, &sz, fp)) != -1) {
		/* skip empty lines and comments. */
		if (!len || buf[0] == '#' || buf[0] == '\n')
			continue;
		if (len > 0 && buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		/* copy and add regex */
		rule = emalloc(sizeof(*rule));

		r = regcomp(&(rule->preg), buf, REG_NOSUB | REG_EXTENDED);
		if (r != 0) {
			regerror(r, &(rule->preg), buf, len);
			weprintf("invalid pattern: %s\n", buf);
			free(buf);
			fclose(fp);
			rej_free(db);
			return -1;
		}

		TAILQ_INSERT_TAIL(&db->rejrule_head, rule, entry);
	}

	if (ferror(fp)) {
		weprintf("%s: read error:", rejpath);
		free(buf);
		fclose(fp);
		rej_free(db);
		return -1;
	}

	free(buf);
	fclose(fp);

	return 0;
}

/* Match pre-computed regexes against the file */
int
rej_match(struct db *db, const char *file)
{
	struct rejrule *rule;

	TAILQ_FOREACH(rule, &db->rejrule_head, entry)
		if (regexec(&rule->preg, file, 0, NULL, 0) != REG_NOMATCH)
			return 1;
	return 0;
}
