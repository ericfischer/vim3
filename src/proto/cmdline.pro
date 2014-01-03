/* cmdline.c */
int getcmdline __PARMS((int firstc, char_u *buff));
void redrawcmdline __PARMS((void));
void compute_cmdrow __PARMS((void));
void redrawcmd __PARMS((void));
int docmdline __PARMS((char_u *cmdline, int sourcing, int repeating));
int autowrite __PARMS((BUF *buf));
void autowrite_all __PARMS((void));
int doecmd __PARMS((int fnum, char_u *fname, char_u *sfname, char_u *command, int hide, linenr_t newlnum));
void gotocmdline __PARMS((int clr));
int check_fname __PARMS((void));
int getfile __PARMS((int fnum, char_u *fname, char_u *sfname, int setpm, linenr_t lnum));
char_u *ExpandOne __PARMS((char_u *str, int list_notfound, int mode));
char_u *addstar __PARMS((char_u *fname, int len));
int dosource __PARMS((register char_u *fname));
void prepare_viminfo_history __PARMS((int len));
int read_viminfo_history __PARMS((char_u *line, linenr_t *lnum, FILE *fp, int force));
void finish_viminfo_history __PARMS((void));
void write_viminfo_history __PARMS((FILE *fp));
