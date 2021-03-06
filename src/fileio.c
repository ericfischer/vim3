/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * fileio.c: read from and write to a file
 */

/*
 * special feature of this version: NUL characters in the file are
 * replaced by newline characters in memory. This allows us to edit
 * binary files!
 */

#ifdef MSDOS
# include <io.h>
#endif

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "fcntl.h"

#ifdef LATTICE
# include <proto/dos.h>		/* for Lock() and UnLock() */
#endif

#define BUFSIZE		8192			/* size of normal write buffer */
#define SBUFSIZE	256				/* size of emergency write buffer */

#ifdef VIMINFO
static void check_marks_read __ARGS((void));
#endif
static int  write_buf __ARGS((int, char_u *, int));

	void
filemess(name, s)
	char_u		*name;
	char_u		*s;
{
		/* careful: home_replace calls vimgetenv(), which also uses IObuff! */
	home_replace(name, IObuff + 1, IOSIZE - 1);
	IObuff[0] = '"';
	STRCAT(IObuff, "\" ");
	STRCAT(IObuff, s);
	/*
	 * For the first message may have to start a new line.
	 * For further ones overwrite the previous one, reset msg_scroll before
	 * calling filemess().
	 */
	msg_start();
	msg_outtrans(IObuff);
	stop_highlight();
	msg_clr_eos();
	flushbuf();
}

/*
 * Read lines from file 'fname' into the buffer after line 'from'.
 *
 * 1. We allocate blocks with lalloc, as big as possible.
 * 2. Each block is filled with characters from the file with a single read().
 * 3. The lines are inserted in the buffer with ml_append().
 *
 * (caller must check that fname != NULL)
 *
 * skip_lnum is the number of lines that must be skipped
 * nlines is the number of lines that are appended
 * When not recovering skip_lnum is 0 and nlines MAXLNUM.
 *
 * return FAIL for failure, OK otherwise
 */
	int
readfile(fname, sfname, from, newfile, skip_lnum, nlines)
	char_u		   *fname;
	char_u		   *sfname;
	linenr_t		from;
	int				newfile;
	linenr_t		skip_lnum;
	linenr_t		nlines;
{
	int 				fd;
	register char_u 	c;
	register linenr_t	lnum = from;
	register char_u 	*ptr = NULL;			/* pointer into read buffer */
	register char_u		*buffer = NULL;			/* read buffer */
	char_u				*new_buffer = NULL;		/* init to shut up gcc */
	char_u				*line_start = NULL;		/* init to shut up gcc */
	colnr_t				len;
	register long		size;
	register char_u		*p;
	long				filesize = 0;
	int					split = 0;				/* number of split lines */
#define UNKNOWN		0x0fffffff					/* file size is unknown */
	linenr_t			linecnt = curbuf->b_ml.ml_line_count;
	int					incomplete = FALSE; 	/* was the last line incomplete? */
	int 				error = FALSE;			/* errors encountered */
	long				linerest = 0;			/* remaining characters in line */
	int					firstpart = TRUE;		/* reading first part */
#ifdef UNIX
	int					perm;
#endif
	int					textmode = curbuf->b_p_tx;		/* accept CR-LF for line break */
	struct stat			st;
	int					readonly;
	int					msg_save;

	/*
	 * If there is no file name yet, use the one for the read file.
	 * b_notedited is set to reflect this.
	 */
	if (curbuf->b_filename == NULL)
	{
		if (setfname(fname, sfname, FALSE) == OK)
			curbuf->b_notedited = TRUE;
	}

	if (sfname == NULL)
		sfname = fname;
	/*
	 * Use the short filename whenever possible.
	 * Avoids problems with networks and when directory names are changed.
	 */
	if (!did_cd)
		fname = sfname;

	if (bufempty())		/* special case: buffer has no lines */
		linecnt = 0;

#ifdef UNIX
	/*
	 * On Unix it is possible to read a directory, so we have to
	 * check for it before the open().
	 */
	perm = getperm(fname);
# ifdef _POSIX_SOURCE
	if (perm >= 0 && !S_ISREG(perm))				/* not a regular file */
# else
	if (perm >= 0 && (perm & S_IFMT) != S_IFREG)	/* not a regular file */
# endif
	{
# ifdef _POSIX_SOURCE
		if (S_ISDIR(perm))
# else
		if ((perm & S_IFMT) == S_IFDIR)
# endif
			filemess(fname, (char_u *)"is a directory");
		else
			filemess(fname, (char_u *)"is not a file");
		return FAIL;
	}
#endif

	if (newfile && !readonlymode)			/* default: set file not readonly */
		curbuf->b_p_ro = FALSE;

	if (newfile && stat((char *)fname, &st) != -1)	/* remember time of file */
		curbuf->b_mtime = st.st_mtime;
	else
		curbuf->b_mtime = 0;

/*
 * for UNIX: check readonly with perm and access()
 * for MSDOS and Amiga: check readonly by trying to open the file for writing
 */
	readonly = FALSE;
#ifdef UNIX
	if (!(perm & 0222) || access((char *)fname, 2))
		readonly = TRUE;
	fd = open((char *)fname, O_RDONLY);
#else
	if (!newfile || readonlymode || (fd = open((char *)fname, O_RDWR)) < 0)
	{
		readonly = TRUE;
		fd = open((char *)fname, O_RDONLY);			/* try to open ro */
	}
#endif

	if (fd < 0)					 /* cannot open at all */
	{
#ifndef UNIX
	/*
	 * On MSDOS and Amiga we can't open a directory, check here.
	 */
		if (isdir(fname) == TRUE)
			filemess(fname, (char_u *)"is a directory");
		else
#endif
			if (newfile)
#ifdef UNIX
				if (perm < 0)
#endif
				{
					filemess(fname, (char_u *)"[New File]");
					return OK;		/* a new file is not an error */
				}
#ifdef UNIX
				else
					filemess(fname, (char_u *)"[Permission Denied]");
#endif

		return FAIL;
	}
	if (newfile && readonly)					/* set file readonly */
		curbuf->b_p_ro = TRUE;

	if (newfile)
		curbuf->b_p_eol = TRUE;

	++no_wait_return;							/* don't wait for return yet */
	if (!recoverymode)
		filemess(fname, (char_u *)"");			/* show that we are busy */

	msg_save = msg_scroll;
	msg_scroll = FALSE;							/* overwrite the file message */
	while (!error && !got_int)
	{
		/*
		 * We allocate as much space for the file as we can get, plus
		 * space for the old line plus room for one terminating NUL.
		 * The amount is limited by the fact that read() only can read
		 * upto max_unsigned characters (and other things).
		 */
#if defined(AMIGA) || defined(MSDOS)
		if (sizeof(int) <= 2 && linerest >= 0x7ff0)
		{
			++split;
			*ptr = NL;				/* split line by inserting a NL */
			size = 1;
		}
		else
#endif
		{
#if !(defined(AMIGA) || defined(MSDOS))
			if (sizeof(int) > 2)
				size = 0x10000L;				/* read 64K at a time */
			else
#endif
			/* If you have 4 byte ints, the compiler may give a warning that
			 * the next statement is not reached. Just ignore it. */
				size = 0x7ff0L - linerest;		/* limit buffer to 32K */

			for ( ; size >= 10; size >>= 1)
			{
				if ((new_buffer = lalloc((long_u)(size + linerest + 1), FALSE)) != NULL)
					break;
			}
			if (new_buffer == NULL)
			{
				do_outofmem_msg();
				error = TRUE;
				break;
			}
			if (linerest)		/* copy characters from the previous buffer */
				memmove((char *)new_buffer, (char *)ptr - linerest, linerest);
			free(buffer);
			buffer = new_buffer;
			ptr = buffer + linerest;
			line_start = buffer;
			
			if ((size = read(fd, (char *)ptr, (size_t)size)) <= 0)
			{
				if (size < 0)				/* read error */
					error = TRUE;
				break;
			}
			filesize += size;				/* count the number of characters */

			/*
			 * when reading the first part of a file: guess EOL type
			 */
			if (firstpart && p_ta)
			{
				for (p = ptr; p < ptr + size; ++p)
					if (*p == NL)
					{
						if (p > ptr && p[-1] == CR)	/* found CR-NL */
							textmode = TRUE;
						else						/* found a single NL */
							textmode = FALSE;
							/* if editing a new file: may set p_tx */
						if (newfile && curbuf->b_p_tx != textmode)
						{
							curbuf->b_p_tx = textmode;
							paramchanged((char_u *)"tx");
						}
						break;
					}
			}
		}

		/*
		 * This loop is executed once for every character read.
		 * Keep it fast!
		 */
		--ptr;
		while (++ptr, --size >= 0)
		{
			if ((c = *ptr) != NUL && c != NL)	/* catch most common case */
				continue;
			if (c == NUL)
				*ptr = NL;		/* NULs are replaced by newlines! */
			else
			{
				if (skip_lnum == 0)
				{
					*ptr = NUL;		/* end of line */
					len = ptr - line_start + 1;
					if (textmode && ptr[-1] == CR)	/* remove CR */
					{
						ptr[-1] = NUL;
						--len;
					}
					if (ml_append(lnum, line_start, len, newfile) == FAIL)
					{
						error = TRUE;
						break;
					}
					++lnum;
					if (--nlines == 0)
					{
						error = TRUE;		/* break loop */
						line_start = ptr;	/* nothing left to write */
						break;
					}
				}
				else
					--skip_lnum;
				line_start = ptr + 1;
			}
		}
		linerest = ptr - line_start;
		firstpart = FALSE;
		breakcheck();
	}

	if (error && nlines == 0)	/* not an error, max. number of lines reached */
		error = FALSE;

	if (!error && !got_int && linerest != 0
#ifdef MSDOS
	/*
	 * in MSDOS textmode ignore a trailing CTRL-Z
	 */
		&& !(!curbuf->b_p_bin && *line_start == Ctrl('Z') &&
											ptr == line_start + 1)
#endif
									)
	{
		/*
		 * If we get EOF in the middle of a line, note the fact and
		 * complete the line ourselves.
		 */
		incomplete = TRUE;
		if (newfile && curbuf->b_p_bin)		/* remember for when writing */
			curbuf->b_p_eol = FALSE;
		*ptr = NUL;
		if (ml_append(lnum, line_start,
						(colnr_t)(ptr - line_start + 1), newfile) == FAIL)
			error = TRUE;
		else
			++lnum;
	}
	if (lnum != from && !newfile)	/* added at least one line */
		CHANGED;

	close(fd);						/* errors are ignored */
	free(buffer);

	--no_wait_return;				/* may wait for return now */
	if (recoverymode)				/* in recovery mode return here */
	{
		msg_scroll = msg_save;
		if (error)
			return FAIL;
#ifdef VIMINFO
		check_marks_read();
#endif /* VIMINFO */
		return OK;
	}

		/* need to delete the last line, which comes from the empty buffer */
	if (newfile && !(curbuf->b_ml.ml_flags & ML_EMPTY))
		ml_delete(curbuf->b_ml.ml_line_count, FALSE);
	linecnt = curbuf->b_ml.ml_line_count - linecnt;
	if (filesize == 0)
		linecnt = 0;
	if (!newfile)
		mark_adjust(from + 1, MAXLNUM, (long)linecnt);

	if (got_int)
	{
		filemess(fname, e_interr);
		msg_scroll = msg_save;
#ifdef VIMINFO
		check_marks_read();
#endif /* VIMINFO */
		return OK;			/* an interrupt isn't really an error */
	}

		/* careful: home_replace calls vimgetenv(), which also uses IObuff! */
	home_replace(fname, IObuff + 1, IOSIZE - 1);
	IObuff[0] = '"';
	sprintf((char *)IObuff + STRLEN(IObuff),
					"\" %s%s%s%s%s%ld line%s, %ld character%s",
			curbuf->b_p_ro ? (p_shm ? "[RO] " : "[readonly] ") : "",
			incomplete ? (p_shm ? "[last]" : "[Incomplete last line] ") : "",
			split ? "[long lines split] " : "",
			error ? "[READ ERRORS] " : "",
#ifdef MSDOS
			textmode ? "" : "[notextmode] ",
#else
			textmode ? "[textmode] " : "",
#endif
			(long)linecnt, plural((long)linecnt),
			filesize, plural(filesize));
	msg_trunc(IObuff);
	msg_scroll = msg_save;

	if (error && newfile)	/* with errors we should not write the file */
	{
		curbuf->b_p_ro = TRUE;
		paramchanged((char_u *)"ro");
	}

	u_clearline();		/* cannot use "U" command after adding lines */

	if (from < curbuf->b_ml.ml_line_count)
	{
		curwin->w_cursor.lnum = from + 1;	/* put cursor at first new line */
		curwin->w_cursor.col = 0;
	}

#ifdef VIMINFO
	check_marks_read();
#endif /* VIMINFO */
	return OK;
}

#ifdef VIMINFO
	static void
check_marks_read()
{
	if (!curbuf->b_marks_read && p_viminfo)
	{
		read_viminfo(NULL, FALSE, TRUE, FALSE);
		curbuf->b_marks_read = TRUE;
	}
}
#endif /* VIMINFO */

/*
 * writeit - write to file 'fname' lines 'start' through 'end'
 *
 * We do our own buffering here because fwrite() is so slow.
 *
 * If forceit is true, we don't care for errors when attempting backups (jw).
 * In case of an error everything possible is done to restore the original file.
 * But when forceit is TRUE, we risk loosing it.
 * When reset_changed is TRUE and start == 1 and end ==
 * curbuf->b_ml.ml_line_count, reset curbuf->b_changed.
 *
 * return FAIL for failure, OK otherwise
 */
	int
buf_write(buf, fname, sfname, start, end, append, forceit, reset_changed)
	BUF				*buf;
	char_u			*fname;
	char_u			*sfname;
	linenr_t		start, end;
	int				append;
	int				forceit;
	int				reset_changed;
{
	int 				fd;
	char_u			   *backup = NULL;
	char_u			   *ffname;
	register char_u	   *s;
	register char_u	   *ptr;
	register char_u		c;
	register int		len;
	register linenr_t	lnum;
	long				nchars;
	char_u				*errmsg = NULL;
	char_u				*buffer;
	char_u				smallbuf[SBUFSIZE];
	char_u				*backup_ext;
	int					bufsize;
	long 				perm = -1;			/* file permissions */
	int					retval = OK;
	int					newfile = FALSE;	/* TRUE if file does not exist yet */
	int					msg_save;
#ifdef UNIX
	struct stat			old;
	int					made_writable = FALSE;	/* 'w' bit has been set */
#endif
#ifdef AMIGA
	BPTR				flock;
#endif
											/* writing everything */
	int					whole = (start == 1 && end == buf->b_ml.ml_line_count);

	if (fname == NULL || *fname == NUL)		/* safety check */
		return FAIL;

	/*
	 * If there is no file name yet, use the one for the written file.
	 * b_notedited is set to reflect this (in case the write fails).
	 */
	if (reset_changed && whole && buf == curbuf && curbuf->b_filename == NULL)
	{
		if (setfname(fname, sfname, FALSE) == OK)
			curbuf->b_notedited = TRUE;
	}

	if (sfname == NULL)
		sfname = fname;
	/*
	 * Use the short filename whenever possible.
	 * Avoids problems with networks and when directory names are changed.
	 */
	ffname = fname;							/* remember full fname */
	if (!did_cd)
		fname = sfname;

		/* make sure we have a valid backup extension to use */
	if (p_bex == NULL || *p_bex == NUL)
		backup_ext = (char_u *)".bak";
	else
		backup_ext = p_bex;

	/*
	 * Disallow writing from .exrc and .vimrc in current directory for
	 * security reasons.
	 */
	if (secure)
	{
		secure = 2;
		emsg(e_curdir);
		return FAIL;
	}

	if (exiting)
		settmode(0);				/* when exiting allow typahead now */

	++no_wait_return;				/* don't wait for return yet */
	filemess(fname, (char_u *)"");	/* show that we are busy */
	msg_save = msg_scroll;
	msg_scroll = FALSE;				/* overwrite the file message */

	buffer = alloc(BUFSIZE);
	if (buffer == NULL)				/* can't allocate big buffer, use small
									 * one (to be able to write when out of
									 * memory) */
	{
		buffer = smallbuf;
		bufsize = SBUFSIZE;
	}
	else
		bufsize = BUFSIZE;

#if defined(UNIX) && !defined(ARCHIE)
		/* get information about original file (if there is one) */
	old.st_dev = old.st_ino = 0;
	if (stat((char *)fname, &old))
		newfile = TRUE;
	else
	{
#ifdef _POSIX_SOURCE
		if (!S_ISREG(old.st_mode))      		/* not a file */
#else
		if ((old.st_mode & S_IFMT) != S_IFREG)	/* not a file */
#endif
		{
#ifdef _POSIX_SOURCE
			if (S_ISDIR(old.st_mode))
#else
			if ((old.st_mode & S_IFMT) == S_IFDIR)
#endif
				errmsg = (char_u *)"is a directory";
			else
				errmsg = (char_u *)"is not a file";
			goto fail;
		}
		perm = old.st_mode;
	}
/*
 * If we are not appending, the file exists, and the 'writebackup', 'backup'
 * or 'patchmode' option is set, try to make a backup copy of the file.
 */
	if (!append && perm >= 0 && (p_wb || p_bk || (p_pm != NULL &&
				*p_pm != NUL)) && (fd = open((char *)fname, O_RDONLY)) >= 0)
	{
		int				bfd, buflen;
		char_u			copybuf[BUFSIZE + 1], *wp;
		int				some_error = FALSE;
		struct stat		new;

		new.st_dev = new.st_ino = 0;

		/*
		 * Unix semantics has it, that we may have a writable file, 
		 * that cannot be recreated with a simple open(..., O_CREAT, ) e.g:
		 *  - the directory is not writable, 
		 *  - the file may be a symbolic link, 
		 *  - the file may belong to another user/group, etc.
		 *
		 * For these reasons, the existing writable file must be truncated and
		 * reused. Creation of a backup COPY will be attempted.
		 */
		if (*p_bdir != '>')			/* try to put .bak in current dir */
		{
			if ((backup = modname(fname, backup_ext)) == NULL)
			{
				some_error = TRUE;			/* out of memory */
				goto nobackup;
			}			
			if (!stat((char *)backup, &new) &&
						new.st_dev == old.st_dev && new.st_ino == old.st_ino)
			{
				/*
				 * may happen when modname gave the same file back.
				 * E.g. silly link, or filename-length reached.
				 * If we don't check here, we either ruin the file when
				 * copying or erase it after writing. jw.
				 */
				free(backup);
				backup = NULL;	/* there is no backup file to delete */
				if (*p_bdir == NUL)
				{
					errmsg = (char_u *)"Invalid backup file (use ! to override)";
					goto nobackup;
				}
			}
			else
				remove((char *)backup);		/* remove old backup, if present */
		}
		if (backup == NULL || (bfd = open((char *)backup, O_WRONLY | O_CREAT, 0666)) < 0)
		{
			/* 
			 * 'backupdir' starts with '>' or  no write/create permission
			 * in current dirr: try again in p_bdir directory. 
			 */
			free(backup);
			wp = gettail(fname);
			sprintf((char *)copybuf, "%s/%s", *p_bdir == '>' ? p_bdir + 1 : p_bdir, wp);
			if ((backup = buf_modname(buf, copybuf, backup_ext)) == NULL)
			{
				some_error = TRUE;			/* out of memory */
				goto nobackup;
			}
			if (!stat((char *)backup, &new) &&
						new.st_dev == old.st_dev && new.st_ino == old.st_ino)
			{
				errmsg = (char_u *)"Invalid backup file (use ! to override)";
				free(backup);
				backup = NULL;	/* there is no backup file to delete */
				goto nobackup;
			}
			remove((char *)backup);
			if ((bfd = open((char *)backup, O_WRONLY | O_CREAT, 0666)) < 0)
			{
				free(backup);
				backup = NULL;	/* there is no backup file to delete */
				errmsg = (char_u *)"Can't make backup file (use ! to override)";
				goto nobackup;
			}
		}
		/* set file protection same as original file, but strip s-bit */
		(void)setperm(backup, perm & 0777);

		/* copy the file. */
		while ((buflen = read(fd, (char *)copybuf, BUFSIZE)) > 0)
		{
			if (write_buf(bfd, copybuf, buflen) == FAIL)
			{
				errmsg = (char_u *)"Can't write to backup file (use ! to override)";
				break;
			}
		}
		if (close(bfd) < 0 && errmsg == NULL)
			errmsg = (char_u *)"Close error for backup file (use ! to override)";
		if (buflen < 0)
			errmsg = (char_u *)"Can't read file for backup (use ! to override)";
nobackup:
		close(fd);				/* ignore errors for closing read file */
	/* ignore errors when forceit is TRUE */
		if ((some_error || errmsg) && !forceit)
		{
			retval = FAIL;
			goto fail;
		}
		errmsg = NULL;
	}
		/* if forceit and the file was read-only: make it writable */
	if (forceit && (old.st_uid == getuid()) && perm >= 0 && !(perm & 0200))
 	{
		perm |= 0200;	
		(void)setperm(fname, perm);
		made_writable = TRUE;
			/* if we are writing to the current file, readonly makes no sense */
		if (fname == buf->b_filename || fname == buf->b_sfilename)
			buf->b_p_ro = FALSE;
 	}
#else /* end of UNIX, start of the rest */

/*
 * If we are not appending, the file exists, and the 'writebackup' or
 * 'backup' option is set, make a backup.
 * Do not make any backup, if "writebackup" and "backup" are 
 * both switched off. This helps when editing large files on
 * almost-full disks. (jw)
 */
	perm = getperm(fname);
	if (perm < 0)
		newfile = TRUE;
	else if (isdir(fname) == TRUE)
	{
		errmsg = (char_u *)"is a directory";
		goto fail;
	}
	if (!append && perm >= 0 && (p_wb || p_bk ||
										(p_pm != NULL && *p_pm != NUL)))
	{
		/*
		 * Form the backup file name - change path/fo.o.h to path/fo.o.h.bak
		 */
		backup = buf_modname(buf, fname, backup_ext);
		if (backup == NULL)
		{
			if (!forceit)
				goto fail;
		}
		else
		{
			/*
			 * Delete any existing backup and move the current version to the
			 * backup.  For safety, we don't remove the backup until the write
			 * has finished successfully. And if the 'backup' option is set,
			 * leave it around.
			 */
#ifdef AMIGA
			/*
			 * With MSDOS-compatible filesystems (crossdos, messydos) it is
			 * possible that the name of the backup file is the same as the
			 * original file. To avoid the chance of accidently deleting the
			 * original file (horror!) we lock it during the remove.
			 * This should not happen with ":w", because startscript() should
			 * detect this problem and set buf->b_shortname, causing modname to
			 * return a correct ".bak" filename. This problem does exist with
			 * ":w filename", but then the original file will be somewhere else
			 * so the backup isn't really important. If autoscripting is off
			 * the rename may fail.
			 */
			flock = Lock((UBYTE *)fname, (long)ACCESS_READ);
#endif
			remove((char *)backup);
#ifdef AMIGA
			if (flock)
				UnLock(flock);
#endif
			if (rename((char *)fname, (char *)backup) != 0)
			{
				if (forceit)
				{
					free(backup);	/* don't do the rename below */
					backup = NULL;
				}
				else
				{
					errmsg = (char_u *)"Can't make backup file (use ! to override)";
					goto fail;
				}
			}
		}
	}
#endif /* UNIX */

	/*
	 * If the original file is being overwritten, there is a small chance that
	 * we crash in the middle of writing. Therefore the file is preserved now.
	 * This makes all block numbers positive so that recovery does not need
	 * the original file.
	 * Don't do this if there is a backup file and we are exiting.
	 */
	if (reset_changed && !newfile && !otherfile(ffname) &&
											!(exiting && backup != NULL))
		ml_preserve(buf, FALSE);

	/* 
	 * We may try to open the file twice: If we can't write to the
	 * file and forceit is TRUE we delete the existing file and try to create
	 * a new one. If this still fails we may have lost the original file!
	 * (this may happen when the user reached his quotum for number of files).
	 * Appending will fail if the file does not exist and forceit is FALSE.
	 */
	while ((fd = open((char *)fname, O_WRONLY | (append ?
					(forceit ? (O_APPEND | O_CREAT) : O_APPEND) :
					(O_CREAT | O_TRUNC)), 0666)) < 0)
 	{
		/*
		 * A forced write will try to create a new file if the old one is
		 * still readonly. This may also happen when the directory is
		 * read-only. In that case the remove() will fail.
		 */
		if (!errmsg)
		{
			errmsg = (char_u *)"Can't open file for writing";
			if (forceit)
			{
#ifdef UNIX
				/* we write to the file, thus it should be marked
													writable after all */
				perm |= 0200;		
				made_writable = TRUE;
				if (old.st_uid != getuid() || old.st_gid != getgid())
					perm &= 0777;
#endif /* UNIX */
				if (!append)		/* don't remove when appending */
					remove((char *)fname);
				continue;
			}
		}
/*
 * If we failed to open the file, we don't need a backup. Throw it away.
 * If we moved or removed the original file try to put the backup in its place.
 */
 		if (backup != NULL)
		{
#ifdef UNIX
			struct stat st;

			/*
			 * There is a small chance that we removed the original, try
			 * to move the copy in its place.
			 * This won't work if the backup is in another file system!
			 * In that case we leave the copy around.
			 */
			 							/* file does not exist */
			if (stat((char *)fname, &st) < 0)
										/* put the copy in its place */
				rename((char *)backup, (char *)fname);
										/* original file does exist */
			if (stat((char *)fname, &st) >= 0)
				remove((char *)backup);	/* throw away the copy */
#else
										/* try to put the original file back */
 			rename((char *)backup, (char *)fname);
#endif
		}
 		goto fail;
 	}
	errmsg = NULL;

	if (end > buf->b_ml.ml_line_count)
		end = buf->b_ml.ml_line_count;
	len = 0;
	s = buffer;
	nchars = 0;
	if (buf->b_ml.ml_flags & ML_EMPTY)
		start = end + 1;
	for (lnum = start; lnum <= end; ++lnum)
	{
		/*
		 * The next while loop is done once for each character written.
		 * Keep it fast!
		 */
		ptr = ml_get_buf(buf, lnum, FALSE) - 1;
		while ((c = *++ptr) != NUL)
		{
			if (c == NL)
				*s = NUL;		/* replace newlines with NULs */
			else
				*s = c;
			++s;
			if (++len != bufsize)
				continue;
			if (write_buf(fd, buffer, bufsize) == FAIL)
			{
				end = 0;				/* write error: break loop */
				break;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		}
			/* write failed or last line has no EOL: stop here */
		if (end == 0 || (buf->b_p_bin && lnum == buf->b_ml.ml_line_count && !buf->b_p_eol))
			break;
		if (buf->b_p_tx)		/* write CR-NL */
		{
			*s = CR;
			++s;
			if (++len == bufsize)
			{
				if (write_buf(fd, buffer, bufsize) == FAIL)
				{
					end = 0;				/* write error: break loop */
					break;
				}
				nchars += bufsize;
				s = buffer;
				len = 0;
			}
		}
		*s = NL;
		++s;
		if (++len == bufsize && end)
		{
			if (write_buf(fd, buffer, bufsize) == FAIL)
			{
				end = 0;				/* write error: break loop */
				break;
			}
			nchars += bufsize;
			s = buffer;
			len = 0;
		}
	}
	if (len && end)
	{
		if (write_buf(fd, buffer, len) == FAIL)
			end = 0;				/* write error */
		nchars += len;
	}

	if (close(fd) != 0)
	{
		errmsg = (char_u *)"Close failed";
		goto fail;
	}
#ifdef UNIX
	if (made_writable)
		perm &= ~0200;			/* reset 'w' bit for security reasons */
#endif
	if (perm >= 0)
		(void)setperm(fname, perm);	/* set permissions of new file same as old file */

	if (end == 0)
	{
		errmsg = (char_u *)"write error (file system full?)";
		/*
		 * If we have a backup file, try to put it in place of the new file,
		 * because it is probably corrupt. This avoids loosing the original
		 * file when trying to make a backup when writing the file a second
		 * time.
		 * For unix this means copying the backup over the new file.
		 * For others this means renaming the backup file.
		 * If this is OK, don't give the extra warning message.
		 */
		if (backup != NULL)
		{
#ifdef UNIX
			char_u		copybuf[BUFSIZE + 1];
			int			bfd, buflen;

			if ((bfd = open((char *)backup, O_RDONLY)) >= 0)
			{
				if ((fd = open((char *)fname,
								O_WRONLY | O_CREAT | O_TRUNC, 0666)) >= 0)
				{
					/* copy the file. */
					while ((buflen = read(bfd, (char *)copybuf, BUFSIZE)) > 0)
						if (write_buf(fd, copybuf, buflen) == FAIL)
							break;
					if (close(fd) >= 0 && buflen == 0)	/* success */
						end = 1;
				}
				close(bfd);		/* ignore errors for closing read file */
			}
#else
			remove((char *)fname);			/* throw away the new file */
			if (rename((char *)backup, (char *)fname) == 0)
				end = 1;
#endif
		}
		goto fail;
	}

	lnum -= start;			/* compute number of written lines */
	--no_wait_return;		/* may wait for return now */

		/* careful: home_replace calls vimgetenv(), which also uses IObuff! */
	home_replace(fname, IObuff + 1, IOSIZE - 1);
	IObuff[0] = '"';
	sprintf((char *)IObuff + STRLEN(IObuff),
					"\"%s%s %ld line%s, %ld character%s",
			newfile ? " [New File]" : "",
#ifdef MSDOS
			buf->b_p_tx ? "" : " [notextmode]",
#else
			buf->b_p_tx ? " [textmode]" : "",
#endif
			(long)lnum, plural((long)lnum),
			nchars, plural(nchars));
	msg_trunc(IObuff);

	if (reset_changed && whole)			/* when written everything */
	{
		UNCHANGED(buf);
		u_unchanged(buf);
		/*
		 * If written to the current file, update the timestamp of the swap file
		 * and reset the 'notedited' flag.
		 */
		if (!exiting && buf->b_filename != NULL &&
							fnamecmp(ffname, buf->b_filename) == 0)
		{
			ml_timestamp(buf);
			buf->b_notedited = FALSE;
		}
	}

	/*
	 * If we kept a backup until now, and we are in patch mode, then we make
	 * the backup file our 'original' file.
	 */
	if (p_pm && *p_pm)
	{
	    char *org = (char *)modname(fname, p_pm);

		if (backup != NULL)
		{
		    struct stat st;

			/*
			 * If the original file does not exist yet
			 * the current backup file becomes the original file
			 */
		    if (org == NULL)
				EMSG("patchmode: can't save original file");
			else if (stat(org, &st) < 0)
			{
			    rename((char *)backup, org);
				free(backup);			/* don't delete the file */
				backup = NULL;
			}
		}
	    /*
		 * If there is no backup file, remember that a (new) file was
		 * created.
		 */
	    else
		{
		    int empty_fd;

			if (org == NULL || (empty_fd = open(org, O_CREAT, 0666)) < 0)
			  EMSG("patchmode: can't touch empty original file");
		    else
			  close(empty_fd);
		}
	    if (org != NULL)
		{
		    setperm((char_u *)org, getperm(fname) & 0777);
			free(org);
		}
	}

	/*
	 * Remove the backup unless 'backup' option is set
	 */
	if (!p_bk && backup != NULL && remove((char *)backup) != 0)
		EMSG("Can't delete backup file");
	
	goto nofail;

fail:
	--no_wait_return;		/* may wait for return now */
nofail:

	free(backup);
	if (buffer != smallbuf)
		free(buffer);

	if (errmsg != NULL)
	{
		/* can't use emsg() here, do something alike */
		if (p_eb)
			beep_flush();			/* also includes flush_buffers() */
		else
			flush_buffers(FALSE);	/* flush internal buffers */
		(void)set_highlight('e');	/* set highlight mode for error messages */
		start_highlight();
		filemess(fname, errmsg);
		retval = FAIL;
		if (end == 0)
		{
			MSG_OUTSTR("\nWARNING: Original file may be lost or damaged\n");
			MSG_OUTSTR("don't quit the editor until the file is sucessfully written!");
		}
	}
	msg_scroll = msg_save;
	return retval;
}

/*
 * write_buf: call write() to write a buffer
 *
 * return FAIL for failure, OK otherwise
 */
	static int
write_buf(fd, buf, len)
	int		fd;
	char_u	*buf;
	int		len;
{
	int		wlen;

	while (len)
	{
		wlen = write(fd, (char *)buf, (size_t)len);
		if (wlen <= 0)				/* error! */
			return FAIL;
		len -= wlen;
		buf += wlen;
	}
	return OK;
}

/*
 * add extention to filename - change path/fo.o.h to path/fo.o.h.ext or
 * fo_o_h.ext for MSDOS or when dotfname option reset.
 *
 * Assumed that fname is a valid name found in the filesystem we assure that
 * the return value is a different name and ends in ".ext".
 * "ext" MUST start with a "." and MUST be at most 4 characters long.
 * Space for the returned name is allocated, must be freed later.
 */

	char_u *
modname(fname, ext)
	char_u *fname, *ext;
{
	return buf_modname(curbuf, fname, ext);
}

	char_u *
buf_modname(buf, fname, ext)
	BUF		*buf;
	char_u *fname, *ext;
{
	char_u				*retval;
	register char_u 	*s;
	register char_u		*ptr;
	register int		fnamelen, extlen;

	extlen = STRLEN(ext);

	/*
	 * if there is no filename we must get the name of the current directory
	 * (we need the full path in case :cd is used)
	 */
	if (fname == NULL || *fname == NUL)
	{
		retval = alloc((unsigned)(MAXPATHL + extlen + 3));
		if (retval == NULL)
			return NULL;
		if (vim_dirname(retval, MAXPATHL) == FAIL ||
									(fnamelen = STRLEN(retval)) == 0)
			return NULL;
		if (!ispathsep(retval[fnamelen - 1]))
		{
			retval[fnamelen++] = PATHSEP;
			retval[fnamelen] = NUL;
		}
	}
	else
	{
		fnamelen = STRLEN(fname);
		retval = alloc((unsigned)(fnamelen + extlen + 1));
		if (retval == NULL)
			return NULL;
		STRCPY(retval, fname);
	}

	/*
	 * search backwards until we hit a '/', '\' or ':' replacing all '.'
	 * by '_' for MSDOS or when dotfname option reset.
	 * Then truncate what is after the '/', '\' or ':' to 8 characters for
	 * MSDOS and 26 characters for AMIGA and UNIX.
	 */
	for (ptr = retval + fnamelen; ptr >= retval; ptr--)
	{
#ifndef MSDOS
		if (buf->b_p_sn || buf->b_shortname)
#endif
			if (*ptr == '.')	/* replace '.' by '_' */
				*ptr = '_';
		if (ispathsep(*ptr))
			break;
	}
	ptr++;

	/* the filename has at most BASENAMELEN characters. */
	if (STRLEN(ptr) > (unsigned)BASENAMELEN)
		ptr[BASENAMELEN] = '\0';
#ifndef MSDOS
	if ((buf->b_p_sn || buf->b_shortname) && STRLEN(ptr) > (unsigned)8)
		ptr[8] = '\0';
#endif
	s = ptr + STRLEN(ptr);

	/*
	 * Append the extention.
	 * ext must start with '.' and cannot exceed 3 more characters.
	 */
	STRCPY(s, ext);
#ifdef MSDOS
	if (fname == NULL || *fname == NUL)		/* can't have just the ext */
		*s = '_';
#endif
	if (fname != NULL && STRCMP(fname, retval) == 0)
	{
		/* after modification still the same name? */
		/* we search for a character that can be replaced by '_' */
		while (--s >= ptr)
		{
			if (*s != '_')
			{
				*s = '_';
				break;
			}
		}
		if (s < ptr)
		{
			/* fname was "________.<ext>" how tricky! */
			*ptr = 'v';
		}
	}
	return retval;
}

/* vim_fgets();
 *
 * Like fgets(), but if the file line is too long, it is truncated and the
 * rest of the line is thrown away.  Returns TRUE or FALSE for end-of-file or
 * not.  The integer pointed to by lnum is incremented.  Note: do not pass
 * IObuff as the buffer since this is used to read and discard the extra part
 * of any long lines.
 */
	int
vim_fgets(buf, size, fp, lnum)
	char_u		*buf;
	int			size;
	FILE		*fp;
	linenr_t	*lnum;
{
	char *eof;

	buf[size - 2] = NUL;
	eof = fgets((char *)buf, size, fp);
	if (buf[size - 2] != NUL && buf[size - 2] != '\n')
	{
		buf[size - 1] = NUL;		/* Truncate the line */

		/* Now throw away the rest of the line: */
		do
		{
			IObuff[IOSIZE - 2] = NUL;
			fgets((char *)IObuff, IOSIZE, fp);
		} while (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != '\n');
		return !VIM_EOF;
	}
	if (lnum != NULL)
		++*lnum;
	return (eof == NULL) ? VIM_EOF : !VIM_EOF;
}

/*
 * rename() only works if both files are on the same file system, this
 * function will (attempts to?) copy the file across if rename fails -- webb
 */
	int
vim_rename(from, to)
	char_u *from;
	char_u *to;
{
	int		fd_in;
	int		fd_out;
	int		n;
	char 	*errmsg = NULL;

/*
 * first try a normal rename
 */
	if (rename((char *)from, (char *)to) == 0)
		return 0;
/*
 * if this fails, try copying the file
 */
	fd_in = open((char *)from, O_RDONLY);
	if (fd_in == -1)
		return -1;
	fd_out = creat((char *)to, 0666);
	if (fd_out == -1)
	{
		close(fd_in);
		return -1;
	}
	while ((n = read(fd_in, IObuff, IOSIZE)) > 0)
		if (write(fd_out, IObuff, n) != n)
		{
			errmsg = "writing to";
			break;
		}
	close(fd_in);
	if (close(fd_out) < 0)
		errmsg = "closing";
	if (n < 0)
	{
		errmsg = "reading";
		to = from;
	}
	if (errmsg != NULL)
	{
		sprintf((char *)IObuff, "Error %s '%s'", errmsg, to);
		emsg(IObuff);
		return -1;
	}
	unlink((char *)from);
	return 0;
}
