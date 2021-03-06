/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */
#define ADDED_BY_WEBB
#ifdef ADDED_BY_WEBB

#define ADDED_BY_WEBB_COMPILE
#define ADDED_BY_WEBB_SIGNALS
#define ADDED_BY_WEBB_ISIDCHAR

#define WEBB_DIAG(str)	{FILE *_fp_ = fopen("out.debug", "a"); fprintf(_fp_, str); fclose(_fp_); }
#define WEBB_DIAG1(str, arg)	{FILE *_fp_ = fopen("out.debug", "a"); fprintf(_fp_, str, arg); fclose(_fp_); }
#define WEBB_DIAG2(str, arg1, arg2)	{FILE *_fp_ = fopen("out.debug", "a"); fprintf(_fp_, str, arg1, arg2); fclose(_fp_); }
#define WEBB_DIAG3(str, arg1, arg2, arg3)	{FILE *_fp_ = fopen("out.debug", "a"); fprintf(_fp_, str, arg1, arg2, arg3); fclose(_fp_); }

#endif /* ADDED_BY_WEBB */

#if defined(SYSV_UNIX) || defined(BSD_UNIX)
# ifndef UNIX
#  define UNIX
# endif
#endif

/*
 * Shorhand for unsinged variables. Many systems, but not all, have u_char
 * already defined, so we use char_u to avoid trouble.
 */
typedef unsigned char	char_u;
typedef unsigned short	short_u;
typedef unsigned int	int_u;
typedef unsigned long	long_u;

#include <stdio.h>
#include <ctype.h>

#if !defined(DOMAIN) && !defined(NOLIMITS)
# include <limits.h>		/* For INT_MAX, remove this if it does not exist */
#endif

#ifdef BSD_UNIX
# ifndef apollo
#  include <strings.h>
# endif
# ifdef __STDC__
#  include <string.h>
# endif
#else
# include <string.h>
#endif

#include "ascii.h"
#include "keymap.h"
#include "term.h"
#include "macros.h"

#ifdef LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#else
# ifdef _DCC
#  include <sys/stat.h>
# else
#  ifdef MSDOS 
#   include <sys\stat.h>
#  else
#   ifdef UNIX
#	 ifndef linux
#	  define volatile		/* needed for gcc */
#	  define signed			/* needed for gcc */
#    endif
#    include <sys/types.h>
#    include <sys/stat.h>
#   else
#     include <stat.h>
#   endif
#  endif
# endif
#endif

#if !defined(DOMAIN) && !defined(NOSTDLIB)
# include <stdlib.h>
#endif

#ifdef AMIGA
/*
 * arpbase.h must be included before functions.h
 */
# include <libraries/arpbase.h>

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
# include <signal.h>
#endif

#ifndef AMIGA
/*
 * For the Amiga we use a version of getenv that does local variables under 2.0
 */
# define vimgetenv(x) (char_u *)getenv((char *)x)
#endif

#ifdef AZTEC_C
# include <functions.h>
# define __ARGS(x)	x
# define __PARMS(x)	x
#endif

#ifdef SASC
# include <clib/exec_protos.h>
# define __ARGS(x)	x
# define __PARMS(x)	x
#endif

#ifdef _DCC
# include <functions.h>
# define __ARGS(x)	x
# define __PARMS(x)	x
#endif
#ifdef __sgi
# define __ARGS(x)	x
# define __PARMS(x) x
#endif

#ifdef __TURBOC__
# define __ARGS(x) x
#endif

#if defined(MSDOS) && !defined(NT)
# include <dos.h>
# include <dir.h>
# include <time.h>
#endif

#ifdef SOLARIS
# include <stdlib.h>
#endif

#ifdef UNIX
# include <unistd.h>		/* any unix that doesn't have it? */
# ifdef SCO
#  undef M_XENIX
#  include <sys/ndir.h>		/* for MAXNAMLEN */
# else
#  if defined(SOLARIS) || defined(AIX) || defined(ARCHIE) || defined(__COHERENT__)
#   include <dirent.h>		/* for MAXNAMLEN */
#  else
#   include <sys/dir.h>		/* for MAXNAMLEN */
#  endif
# endif
# ifdef USL
#  define MAXNAMLEN DIRSIZ
# endif
# if defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
#  define MAXNAMLEN UFS_MAXNAMLEN		/* for dynix/ptx */
# endif
# if defined(NAME_MAX) && !defined(MAXNAMLEN)
#  define MAXNAMLEN NAME_MAX			/* for Linux before .99p3 */
# endif
/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 * 	     for not being able to open the swap file.
 */
# if !defined(MAXNAMLEN)
#  define MAXNAMLEN 512                 /* for all other Unix */
# endif
#endif

#ifdef UNICOS		/* would make sense for other systems too */
# include <errno.h>
#endif

#if defined(__STDC__) || defined(__GNUC__)
# ifndef __ARGS
#  define __ARGS(x) x
# endif /* __ARGS */
# if defined(_SEQUENT_)
#  include "ptx_stdlib.h"
# endif
# if defined(sun) && !defined(SOLARIS)
#  include "sun_stdlib.h"
# endif
#else /*__STDC__*/
# if defined(_SEQUENT_) && !defined(_STDLIB_H_)
  extern char *getenv();
  extern void *malloc();
# endif
#endif /* __STDC__ */

#ifdef __COHERENT__
#undef __ARGS
#endif /* __COHERENT__ */

#ifdef __ARGS
# ifndef __PARMS
#  define __PARMS(x) x
# endif
#endif
#ifndef __ARGS
#define __ARGS(x)	()
#endif
#ifndef __PARMS
#define __PARMS(x)	()
#endif

/*
 * for systems that do not allow free(NULL)
 */
#ifdef NO_FREE_NULL
# define free(x)	nofreeNULL(x)
  extern void nofreeNULL __ARGS((void *));
#endif

/*
 * fnamecmp() is used to compare filenames.
 * On some systems case in a filename does not matter, on others it does.
 * (this does not account for maximum name lengths, thus it is not 100% accurate!)
 */
#if defined(AMIGA) || defined(MSDOS)
# define fnamecmp(x, y) stricmp((char *)(x), (char *)(y))
#else
# define fnamecmp(x, y) strcmp((char *)(x), (char *)(y))
#endif

/*
 * flags for updateScreen()
 * The higher the value, the higher the priority
 */
#define VALID					10	/* buffer not changed */
#define INVERTED				20	/* redisplay inverted part */
#define VALID_TO_CURSCHAR		30	/* buffer at/below cursor changed */
#define NOT_VALID				40	/* buffer changed */
#define CURSUPD					50	/* buffer changed, update cursor first */
#define CLEAR					60	/* screen messed up, clear it */

/* values for State */
/*
 * The lowest four bits are used to distinguish normal/visual/cmdline/
 * insert+replace mode. This is used for mapping. If none of these bits is set
 * no mapping is done.
 * The upper four bits are used to distinguish between other states.
 */
#define NORMAL					0x01
#define VISUAL					0x02
#define CMDLINE 				0x04
#define INSERT					0x08
#define NORMAL_BUSY				0x11	/* busy interpreting a command */
#define REPLACE 				0x28	/* replace mode */
#define NOMAPPING 				0x40	/* no :mapping mode for vgetc() */
#define ONLYKEY 				0x50	/* like NOMAPPING, but keys allowed */
#define HITRETURN				0x61	/* waiting for a return */
#define ASKMORE					0x70	/* Asking if you want --more-- */
#define SETWSIZE				0x80	/* window size has changed */
#define ABBREV					0x90	/* abbreviation instead of mapping */

/* directions */
#define FORWARD 				1
#define BACKWARD				-1
#define BOTH_DIRECTIONS			2

/* return values for functions */
#define OK						1
#define FAIL					0

/* for GetChars */
#define T_PEEK					1	/* do not wait at all */
#define T_WAIT					2	/* wait for a short time */
#define T_BLOCK					3	/* wait forever */

/*
 * values for command line completion
 */
#define CONTEXT_UNKNOWN			-2
#define EXPAND_UNSUCCESSFUL		-1
#define EXPAND_NOTHING			0
#define EXPAND_COMMANDS			1
#define EXPAND_FILES			2
#define EXPAND_DIRECTORIES		3
#define EXPAND_SETTINGS			4
#define EXPAND_BOOL_SETTINGS	5
#define EXPAND_TAGS				6
#define EXPAND_OLD_SETTING		7
#define EXPAND_HELP				8

/* Values for the find_pattern_in_path() function args 'type' and 'action': */
#define FIND_ANY		1
#define FIND_DEFINE		2
#define CHECK_PATH		3

#define ACTION_SHOW		1
#define ACTION_GOTO		2
#define ACTION_SPLIT	3
#define ACTION_SHOW_ALL	4
#define ACTION_EXPAND	5

/*
 * Boolean constants
 */
#ifndef TRUE
#define FALSE	(0)			/* note: this is an int, not a long! */
#define TRUE	(1)
#endif
#define MAYBE	(2)			/* for beginline() and the 'sol' option */

/* May be returned by add_new_completion(): */
#define RET_ERROR				-1

/* Returned by vim_fgets(): */
#define VIM_EOF					1

/* Formatting options for the p_fo variable: */
#define FO_WRAP			't'
#define FO_WRAP_COMS	'c'
#define FO_RET_COMS		'r'
#define FO_OPEN_COMS	'o'
#define FO_Q_COMS		'q'
#define FO_COMS_PADDED	's'
#define FO_Q_SECOND		'2'

#define FO_DFLT			"tcqs"

/*
 * Maximum and minimum screen size (height is unlimited)
 */
#ifdef UNIX
# define MAX_COLUMNS 	1024L
#else
# define MAX_COLUMNS 	255L
#endif
#define MIN_COLUMNS		12		/* minimal columns for screen */
#define MIN_ROWS		1		/* minimal rows for one window */
#define STATUS_HEIGHT	1		/* height of a status line under a window */

/*
 * Buffer sizes
 */
#ifdef UNIX		/* Unix has plenty of memory */
# define CMDBUFFSIZE	1024	/* size of the command processing buffer */
#else
# define CMDBUFFSIZE	256		/* size of the command processing buffer */
#endif

#define LSIZE		512			/* max. size of a line in the tags file */

#define IOSIZE	   (1024+1) 	/* file i/o and sprintf buffer size */
#define MSG_BUF_LEN	80			/* lenght of buffer for small messages */

#define	TERMBUFSIZE	1024

#ifdef linux
# define TBUFSZ 2048			/* buffer size for termcap entry */
#else
# define TBUFSZ 1024			/* buffer size for termcap entry */
#endif

/*
 * maximum length of a file name path
 */
#ifdef UNIX
# define MAXPATHL	1024		/* Unix has long paths and plenty of memory */
#else
# define MAXPATHL	128			/* not too long to put name on stack */
#endif

/*
 * Maximum length of key sequence to be mapped.
 * Must be able to hold an Amiga resize report.
 */
#define MAXMAPLEN	50

#ifdef MSDOS
# define WRITEBIN	"wb"		/* no CR-LF translation */
# define READBIN	"rb"
# define APPENDBIN	"ab"
#else
# define WRITEBIN	"w"
# define READBIN	"r"
# define APPENDBIN	"a"
#endif

#define CHANGED   		set_Changed()
#define UNCHANGED(buf)	unset_Changed(buf)

/*
 * defines to avoid typecasts from (char_u *) to (char *) and back
 */
#define STRCHR(s, c)		(char_u *)strchr((char *)(s), c)
#define STRRCHR(s, c)		(char_u *)strrchr((char *)(s), c)
#define STRLEN(s)			strlen((char *)(s))
#define STRCPY(d, s)		strcpy((char *)(d), (char *)(s))
#define STRNCPY(d, s, n)	strncpy((char *)(d), (char *)(s), n)
#define STRCMP(d, s)		strcmp((char *)(d), (char *)(s))
#define STRNCMP(d, s, n)	strncmp((char *)(d), (char *)(s), n)
#define STRCAT(d, s)		strcat((char *)(d), (char *)(s))

#define MSG(s)				msg((char_u *)(s))
#define EMSG(s)				emsg((char_u *)(s))
#define EMSG2(s, p)			emsg2((char_u *)(s), (char_u *)(p))
#define OUTSTR(s)			outstr((char_u *)(s))
#define OUTSTRN(s)			outstrn((char_u *)(s))
#define MSG_OUTSTR(s)		msg_outstr((char_u *)(s))

typedef long			linenr_t;	/* line number type */
typedef unsigned		colnr_t;	/* column number type */

#define MAXLNUM (0x7fffffff)		/* maximum (invalid) line number */
#ifdef INT_MAX
# define MAXCOL	INT_MAX				/* maximum column number */
#else
# define MAXCOL	32767				/* maximum column number, 15 bits */
#endif

#define SHOWCMD_COLS 10				/* columns needed by shown command */

/*
 * Some versions of isspace() handle Meta characters like a space!
 * This define fixes that.
 */
#ifdef VIM_ISSPACE
# ifdef isspace
#  undef isspace
# endif /* isspace */
# define isspace(x)  (((x) >= 9 && (x) <= 13) || ((x) == 32))
#endif /* VIM_ISSPACE */

/*
 * iswhite() is used for "^" and the like
 */
#define iswhite(x)	((x) == ' ' || (x) == '\t')

#include "structs.h"		/* file that defines many structures */

#ifdef AMIGA
# include "amiga.h"
#endif

#ifdef ARCHIE
# include "archie.h"
#endif

#ifdef MSDOS
# include "msdos.h"
#endif

#ifdef UNIX
# include "unix.h"
#endif
