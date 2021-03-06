/* amiga.c */
void win_resize_on __PARMS((void));
void win_resize_off __PARMS((void));
void mch_write __PARMS((char_u *p, int len));
int GetChars __PARMS((char_u *buf, int maxlen, int time));
int mch_char_avail __PARMS((void));
void sleep __PARMS((int n));
long mch_avail_mem __PARMS((int special));
void vim_delay __PARMS((void));
void mch_suspend __PARMS((void));
void mch_windinit __PARMS((void));
void check_win __PARMS((int argc, char **argv));
void fname_case __PARMS((char_u *name));
void mch_settitle __PARMS((char_u *title, char_u *icon));
void mch_restore_title __PARMS((int which));
int vim_dirname __PARMS((char_u *buf, int len));
int FullName __PARMS((char_u *fname, char_u *buf, int len));
int isFullName __PARMS((char_u *fname));
long getperm __PARMS((char_u *name));
int setperm __PARMS((char_u *name, long perm));
int isdir __PARMS((char_u *name));
void mch_windexit __PARMS((int r));
void mch_settmode __PARMS((int raw));
int mch_screenmode __PARMS((char_u *arg));
int mch_get_winsize __PARMS((void));
void mch_set_winsize __PARMS((void));
int call_shell __PARMS((char_u *cmd, int filter, int cooked));
void breakcheck __PARMS((void));
long Chk_Abort __PARMS((void));
int ExpandWildCards __PARMS((int num_pat, char_u **pat, int *num_file, char_u ***file, int files_only, int list_notfound));
void FreeWild __PARMS((int num, char_u **file));
int has_wildcard __PARMS((char_u *p));
char_u *vimgetenv __PARMS((char_u *var));
