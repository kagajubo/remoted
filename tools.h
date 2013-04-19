#ifndef _TOOLS_H
#define _TOOLS_H

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR '/'
#endif

/* Miscellaneous constants	*/
#define PATH_MAX 	1024
#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */

#define ARRAY_SIZE(array) (sizeof (array) / sizeof (*(array)))

void log_init (const char *logfile);
void logprintf (const char *fmt, ...);

void err_ret (const char *fmt, ...);
void err_sys (const char *fmt, ...);
void err_dump (const char *fmt, ...);
void err_msg (const char *fmt, ...);
void err_quit (const char *fmt, ...);

void *xmalloc (char *var_name, size_t size);
void *xrealloc (void *obj, size_t size);
char *xstrdup (const char *s);
char *xmemdup (const char *mem, int byte_size);

char *strdupdelim (const char *beg, const char *end);
char **sepstring (const char *s);
void path_simplify (char *path);
char *suffix (const char *str);
char *read_whole_line(FILE *fp);
void free_vec (char **vec);
char **merge_vecs (char **v1, char **v2);
int numdigit (long a);
void long_to_string (char *buffer, long number);
int match_backwards (const char *string, const char *pattern);

char *strchug (char *string);	/* removes leading spaces */
char *strchomp (char *string);	/* removes trailing spaces */
/* removes leading & trailing spaces */
#define strstrip( string )	strchomp (strchug (string))
#define trim(string)	strchomp (strchug (string))

char *strjoin (const char *separator, ...);
char **strsplit (const char *string, const char *delimiter, int max_tokens);
void strfreev (char **str_array);
char *strjoinv (const char *separator, char **str_array);
void sys_date (char *DateTime);

#endif

