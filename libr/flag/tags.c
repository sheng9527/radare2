/* radare - LGPL - Copyright 2018 - pancake */

#include <r_flag.h>

R_API RList *r_flag_tags_set(RFlag *f, const char *name, const char *words) {
	r_return_val_if_fail (f && name && words, NULL);
	const char *k = sdb_fmt ("tag.%s", name);
	sdb_set (f->tags, k, words, -1);
	return NULL;
}

R_API RList *r_flag_tags_list(RFlag *f) {
	r_return_val_if_fail (f, NULL);
	RList *res = r_list_newf (free);
	SdbList *o = sdb_foreach_list (f->tags, false);
	SdbListIter *iter;
	SdbKv *kv;
	ls_foreach (o, iter, kv) {
		const char *tag = sdbkv_key (kv);
		if (strlen (tag) < 5) {
			continue;
		}
		r_list_append (res, (void *)strdup (tag + 4));
	}
	ls_free (o);
	return res;
}

R_API void r_flag_tags_reset(RFlag *f, const char *name) {
	// TODO: use name
	r_return_if_fail (f);
	sdb_reset (f->tags);
}

struct iter_glob_flag_t {
	RList *res;
	RList *words;
};

static bool iter_glob_flag(RFlagItem *fi, void *user) {
	struct iter_glob_flag_t *u = (struct iter_glob_flag_t *)user;
	RListIter *iter;
	const char *word;

	r_list_foreach (u->words, iter, word) {
		if (r_str_glob (fi->name, word)) {
			r_list_append (u->res, fi);
		}
	}
	return true;
}

R_API RList *r_flag_tags_get(RFlag *f, const char *name) {
	r_return_val_if_fail (f && name, NULL);
	const char *k = sdb_fmt ("tag.%s", name);
	char *words = sdb_get (f->tags, k, NULL);
	RList *res = r_list_newf (NULL);
	RList *list = r_str_split_list (words, " ");
	struct iter_glob_flag_t u = { .res = res, .words = list };
	r_flag_foreach (f, iter_glob_flag, &u);
	r_list_free (list);
	return res;
}
