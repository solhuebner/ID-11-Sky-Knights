void helper_draw_sprite(byte x, byte y, const prog_uchar *sprite, byte frame);
void helper_draw_char(const prog_uchar *font, char x, char y, char c);
void helper_print(const prog_uchar *font, char x, char y, char* s);
void helper_print_number(const prog_uchar *font, char x, char y, unsigned long n);
void helper_print_p(const prog_uchar *font, char x, char y, const char *s);
void helper_draw_big_char (const prog_uchar *font, char x, char y, char c);
void helper_print_big_number(const prog_uchar *font, char x, char y, unsigned long n, char pad);
void helper_print_big (const prog_uchar *font, char x, char y, char *s);
