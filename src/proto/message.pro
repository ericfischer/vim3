/* message.c */
int msg __PARMS((char_u *s));
int emsg __PARMS((char_u *s));
int emsg2 __PARMS((char_u *s, char_u *a1));
int msg_trunc __PARMS((char_u *s));
void wait_return __PARMS((int redraw));
void msg_start __PARMS((void));
void msg_pos __PARMS((int row, int col));
void msg_outchar __PARMS((int c));
void msg_outnum __PARMS((long n));
int msg_outtrans __PARMS((register char_u *str, register int len));
int msg_outtrans_meta __PARMS((register char_u *str, register int all));
char_u **get_key_names __PARMS((void));
void msg_prt_line __PARMS((char_u *s));
void msg_outstr __PARMS((char_u *s));
void msg_moremsg __PARMS((void));
void msg_clr_eos __PARMS((void));
int msg_end __PARMS((void));
int msg_check __PARMS((void));
