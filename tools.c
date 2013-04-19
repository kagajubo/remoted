#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

#include "tools.h"

static char logfile[PATH_MAX + 1];

void
log_init (const char *file)
{
  /*snprintf (logfile, PATH_MAX + 1, "%s", file);*/
	sprintf (logfile, "%s", file);
}

static void
logvprintf (const char *fmt, va_list args)
{
  FILE *logfp;
  time_t tm;
  char strtime[20];

  tm = time (NULL);
  /*strftime (strtime, 20, "%Y-%m-%d %X", localtime (&tm));*/
  strcpy(strtime,asctime(localtime(&tm))) ; /*test*/
  logfp = fopen (logfile, "a");
  if (logfp == NULL) {
    logfp = fopen (logfile, "w");
    if (logfp == NULL) {
      fprintf (stderr, "%s  ", strtime);
      vfprintf (stderr, fmt, args);
      fprintf (stderr, "fopen error %s, %s\n", logfile, strerror (errno));
      return;
    }
  }

  fprintf (logfp, "%s  ", strtime);
  vfprintf (logfp, fmt, args);
  fclose (logfp);
}

void
logprintf (const char *fmt, ...)
{
  va_list args;

  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  logvprintf (fmt, args);
  va_end (args);
}


static void
memfatal (const char *s)
{
  logprintf ("%s: Not enough memory.\n", s);
  exit (1);
}

/*
void *
xmalloc (size_t size)
{
  void *res;

  res = malloc (size);
  if (!res)
    memfatal ("malloc");
  return res;
}
*/
void *
xmalloc (char *var_name, size_t size)
{
  void *res;

  res = malloc (size);
  if (!res) {
    printf ("%s\t", var_name);
    memfatal ("malloc");
  }
  return res;
}

void *
xrealloc (void *obj, size_t size)
{
  void *res;

  if (obj)
    res = realloc (obj, size);
  else
    res = malloc (size);
  if (!res)
    memfatal ("realloc");
  return res;
}

char *
xstrdup (const char *s)
{
  int l = strlen (s);
  char *s1 = malloc (l + 1);

  if (!s1)
    memfatal ("strdup");
  memcpy (s1, s, l + 1);
  return s1;
}

char *
xmemdup (const char *mem, int byte_size)
{
  char *new_mem;

  new_mem = malloc (byte_size);
  if (!new_mem)
    memfatal ("malloc");
  memcpy (new_mem, mem, byte_size);

  return new_mem;
}

/* Print a message and return to caller. */
static void
err_doit (int errnoflag, const char *fmt, va_list ap)
{
  int errno_save, n;
  char buf[MAXLINE];

  errno_save = errno;       /* value caller might want printed */
  /*vsnprintf (buf, sizeof (buf), fmt, ap);   /* this is safe */
  n = strlen (buf);
  if (errnoflag)
	sprintf (buf + n, ": %s", strerror (errno_save));
/*    snprintf (buf + n, sizeof (buf) - n, ": %s", strerror (errno_save));	*/
  logprintf ("%s\n", buf);
  return;
}

/* Nonfatal error related to a system call.
   Print a message and return. */
void
err_ret (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  err_doit (1, fmt, ap);
  va_end (ap);
  return;
}

/* Fatal error related to a system call.
   Print a message and terminate. */
void
err_sys (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  err_doit (1, fmt, ap);
  va_end (ap);
  exit (1);
}

/* Fatal error related to a system call.
   Print a message, dump core, and terminate. */
void
err_dump (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  err_doit (1, fmt, ap);
  va_end (ap);
  abort ();         /* dump core and terminate */
  exit (1);         /* shouldn't get here */
}

/* Nonfatal error unrelated to a system call.
   Print a message and return. */
void
err_msg (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  err_doit (0, fmt, ap);
  va_end (ap);
  return;
}

/* Fatal error unrelated to a system call.
   Print a message and terminate. */
void
err_quit (const char *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  err_doit (0, fmt, ap);
  va_end (ap);
  exit (1);
}


/* Copy the string formed by two pointers (one on the beginning, other
   on the char after the last char) to a new, malloc-ed location.
   0-terminate it.  */
char *
strdupdelim (const char *beg, const char *end)
{
  char *res = (char *) xmalloc ("res", end - beg + 1);
  memcpy (res, beg, end - beg);
  res[end - beg] = '\0';
  return res;
}

/* Parse a string containing comma-separated elements, and return a
   vector of char pointers with the elements.  Spaces following the
   commas are ignored.  */
char **
sepstring (const char *s)
{
  char **res;
  const char *p;
  int i = 0;

  if (!s || !*s)
    return NULL;
  res = NULL;
  p = s;
  while (*s) {
    if (*s == ',') {
      res = (char **) xrealloc (res, (i + 2) * sizeof (char *));
      res[i] = strdupdelim (p, s);
      res[++i] = NULL;
      ++s;
      /* Skip the blanks following the ','.  */
      while (isspace (*s))
    ++s;
      p = s;
    } else
      ++s;
  }
  res = (char **) xrealloc (res, (i + 2) * sizeof (char *));
  res[i] = strdupdelim (p, s);
  res[i + 1] = NULL;
  return res;
}

/* Canonicalize PATH, and return a new path.  The new path differs from PATH
   in that:
   Multple `/'s are collapsed to a single `/'.
   Leading `./'s and trailing `/.'s are removed.
   Trailing `/'s are removed.
   Non-leading `../'s and trailing `..'s are handled by removing
   portions of the path.

   E.g. "a/b/c/./../d/.." will yield "a/b".  This function originates
   from GNU Bash.

   Changes for Wget:
   Always use '/' as stub_char.
   Don't check for local things using canon_stat.
   Change the original string instead of strdup-ing.
   React correctly when beginning with `./' and `../'.  */
void
path_simplify (char *path)
{
  register int i, start, ddot;
  char stub_char;

  if (!*path)
    return;

  /*stub_char = (*path == '/') ? '/' : '.'; */
  stub_char = '/';

  /* Addition: Remove all `./'-s preceding the string.  If `../'-s
     precede, put `/' in front and remove them too.  */
  i = 0;
  ddot = 0;
  while (1) {
    if (path[i] == '.' && path[i + 1] == '/')
      i += 2;
    else if (path[i] == '.' && path[i + 1] == '.' && path[i + 2] == '/') {
      i += 3;
      ddot = 1;
    } else
      break;
  }
  if (i)
    strcpy (path, path + i - ddot);

  /* Replace single `.' or `..' with `/'.  */
  if ((path[0] == '.' && path[1] == '\0')
      || (path[0] == '.' && path[1] == '.' && path[2] == '\0')) {
    path[0] = stub_char;
    path[1] = '\0';
    return;
  }
  /* Walk along PATH looking for things to compact.  */
  i = 0;
  while (1) {
    if (!path[i])
      break;

    while (path[i] && path[i] != '/')
      i++;

    start = i++;

    /* If we didn't find any slashes, then there is nothing left to do.  */
    if (!path[start])
      break;

    /* Handle multiple `/'s in a row.  */
    while (path[i] == '/')
      i++;

    if ((start + 1) != i) {
      strcpy (path + start + 1, path + i);
      i = start + 1;
    }

    /* Check for trailing `/'.  */
    if (start && !path[i]) {
    zero_last:
      path[--i] = '\0';
      break;
    }

    /* Check for `../', `./' or trailing `.' by itself.  */
    if (path[i] == '.') {
      /* Handle trailing `.' by itself.  */
      if (!path[i + 1])
    goto zero_last;

      /* Handle `./'.  */
      if (path[i + 1] == '/') {
    strcpy (path + i, path + i + 1);
    i = (start < 0) ? 0 : start;
    continue;
      }

      /* Handle `../' or trailing `..' by itself.  */
      if (path[i + 1] == '.' && (path[i + 2] == '/' || !path[i + 2])) {
    while (--start > -1 && path[start] != '/');
    strcpy (path + start + 1, path + i + 2);
    i = (start < 0) ? 0 : start;
    continue;
      }
    }               /* path == '.' */
  }             /* while */

  if (!*path) {
    *path = stub_char;
    path[1] = '\0';
  }
}

/* Return the malloc-ed suffix of STR.  For instance:
   suffix ("foo.bar")       -> "bar"
   suffix ("foo.bar.baz")   -> "baz"
   suffix ("/foo/bar")      -> NULL
   suffix ("/foo.bar/baz")  -> NULL  */
char *
suffix (const char *str)
{
  int i;

  for (i = strlen (str); i && str[i] != '/' && str[i] != '.'; i--);
  if (str[i++] == '.')
    return xstrdup (str + i);
  else
    return NULL;
}

/* Read a line from FP.  The function reallocs the storage as needed
   to accomodate for any length of the line.  Reallocs are done
   storage exponentially, doubling the storage after each overflow to
   minimize the number of calls to realloc().

   It is not an exemplary of correctness, since it kills off the
   newline (and no, there is no way to know if there was a newline at
   EOF).  */
char *
read_whole_line (FILE * fp)
{
  char *line;
  int i, bufsize, c;

  i = 0;
  bufsize = 40;
  line = (char *) xmalloc ("line", bufsize);
  /* Construct the line.  */
  while ((c = getc (fp)) != EOF && c != '\n') {
    if (i > bufsize - 1)
      line = (char *) xrealloc (line, (bufsize <<= 1));
    line[i++] = c;
  }
  if (c == EOF && !i) {
    free (line);
    return NULL;
  }
  /* Check for overflow at zero-termination (no need to double the
     buffer in this case.  */
  if (i == bufsize)
    line = (char *) xrealloc (line, i + 1);
  line[i] = '\0';
  return line;
}

/* Free the pointers in a NULL-terminated vector of pointers, then
   free the pointer itself.  */
void
free_vec (char **vec)
{
  if (vec) {
    char **p = vec;
    while (*p)
      free (*p++);
    free (vec);
  }
}

/* Append vector V2 to vector V1.  The function frees V2 and
   reallocates V1 (thus you may not use the contents of neither
   pointer after the call).  If V1 is NULL, V2 is returned.  */
char **
merge_vecs (char **v1, char **v2)
{
  int i, j;

  if (!v1)
    return v2;
  if (!v2)
    return v1;
  if (!*v2) {
    /* To avoid j == 0 */
    free (v2);
    return v1;
  }
  /* Count v1.  */
  for (i = 0; v1[i]; i++);
  /* Count v2.  */
  for (j = 0; v2[j]; j++);
  /* Reallocate v1.  */
  v1 = (char **) xrealloc (v1, (i + j + 1) * sizeof (char **));
  memcpy (v1 + i, v2, (j + 1) * sizeof (char *));
  free (v2);
  return v1;
}

/* Count the digits in a (long) integer.  */
int
numdigit (long a)
{
  int res = 1;
  while ((a /= 10) != 0)
    ++res;
  return res;
}

/* Print NUMBER to BUFFER.  The digits are first written in reverse
   order (the least significant digit first), and are then reversed.  */
void
long_to_string (char *buffer, long number)
{
  char *p;
  int i, l;

  if (number < 0) {
    *buffer++ = '-';
    number = -number;
  }
  p = buffer;
  /* Print the digits to the string.  */
  do {
    *p++ = number % 10 + '0';
    number /= 10;
  }
  while (number);
  /* And reverse them.  */
  l = p - buffer - 1;
  for (i = l / 2; i >= 0; i--) {
    char c = buffer[i];
    buffer[i] = buffer[l - i];
    buffer[l - i] = c;
  }
  buffer[l + 1] = '\0';
}

/* Match the end of STRING against PATTERN.  For instance:

   match_backwards ("abc", "bc") -> 1
   match_backwards ("abc", "ab") -> 0
   match_backwards ("abc", "abc") -> 1 */
int
match_backwards (const char *string, const char *pattern)
{
  int i, j;

  for (i = strlen (string), j = strlen (pattern); i >= 0 && j >= 0; i--, j--)
    if (string[i] != pattern[j])
      break;
  /* If the pattern was exhausted, the match was succesful.  */
  if (j == -1)
    return 1;
  else
    return 0;
}


/* blame Elliot for these next five routines */

/*trim left space*/
char *
strchug (char *string)
{
  char *start;

  for (start = string; *start && isspace (*start); start++);

  strcpy (string, start);

  return string;
}
/*trim right space*/
char *
strchomp (char *string)
{
  char *s;

  if (!*string)
    return string;

  for (s = string + strlen (string) - 1; s >= string && isspace (*s); s--)
    *s = '\0';

  return string;
}

char *
strjoin (const char *separator, ...)
{
  char *string, *s;
  va_list args;
  int len;
  int separator_len;

  if (separator == NULL)
    separator = "";

  separator_len = strlen (separator);

  va_start (args, separator);

  s = va_arg (args, char *);

  if (s) {
    len = strlen (s) + 1;

    while ((s = va_arg (args, char *))) {
      len += separator_len + strlen (s);
    }
    va_end (args);

    string = xmalloc ("string", len);

    va_start (args, separator);

    *string = 0;
    s = va_arg (args, char *);
    strcat (string, s);

    while ((s = va_arg (args, char *))) {
      strcat (string, separator);
      strcat (string, s);
    }

  } else
    string = xstrdup ("");

  va_end (args);

  return string;
}

char **
strsplit (const char *string, const char *delimiter, int max_tokens)
{
  char **str_array, *s;
  int i;

  if (max_tokens < 1)
    max_tokens = INT_MAX;

  i = 0;
  str_array = NULL;

  s = strstr (string, delimiter);
  if (s) {
    int delimiter_len = strlen (delimiter);

    do {
      int len;
      char *new_string;

      len = s - string;
      new_string = xmalloc ("new_string", len + 1);
      strncpy (new_string, string, len);
      new_string[len] = 0;
      str_array = (char **) xrealloc (str_array, (i + 2) * sizeof (char *));
      str_array[i] = new_string;
      str_array[++i] = NULL;

      string = s + delimiter_len;
      s = strstr (string, delimiter);
    }
    while (--max_tokens && s);
  }
  if (*string) {
    str_array = (char **) xrealloc (str_array, (i + 2) * sizeof (char *));
    str_array[i] = xstrdup (string);
    str_array[++i] = NULL;
  }

  return str_array;
}

void
strfreev (char **str_array)
{
  if (str_array) {
    int i;

    for (i = 0; str_array[i] != NULL; i++)
      free (str_array[i]);

    free (str_array);
  }
}

char *
strjoinv (const char *separator, char **str_array)
{
  char *string;

  if (separator == NULL)
    separator = "";

  if (*str_array) {
    int i, len;
    int separator_len;

    separator_len = strlen (separator);
    len = 1 + strlen (str_array[0]);
    for (i = 1; str_array[i] != NULL; i++)
      len += separator_len + strlen (str_array[i]);

    string = xmalloc ("string", len);
    *string = 0;
    strcat (string, *str_array);
    for (i = 1; str_array[i] != NULL; i++) {
      strcat (string, separator);
      strcat (string, str_array[i]);
    }
  } else
    string = xstrdup ("");

  return string;
}

void
sys_date (char *DateTime)
{
  time_t tm;

  tm = time (NULL);
  /*strftime (DateTime, 15, "%Y%m%d%H%M%S", localtime (&tm));*/
}

