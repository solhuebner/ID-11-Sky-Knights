#include <avr/pgmspace.h>
#include <Arduino.h>
#include <Gamby.h>

extern GambyGraphicsMode gamby;

//Draws the sprite only where pixels are specified
//(saves having to store a mask for now)

void helper_draw_sprite(byte x, byte y, const prog_uchar *sprite, byte frame) {
  unsigned int w = pgm_read_byte_near(sprite);
  unsigned int h = pgm_read_byte_near(++sprite);

  byte this_byte;
  byte this_bit=8;
  byte draw_bit = 0;
  // TODO: This can probably be optimized.
  if (frame > 0) {
    unsigned int pixelsPerFrame = h * w;
    // Round up to the nearest 8 bits
    sprite += ((pixelsPerFrame & 0x7 ? ((pixelsPerFrame &  0xfff8) + 8) : pixelsPerFrame) >> 3) * frame;
  }

  for (byte i=0; i<w; i++) {
    for (byte j=h; j>0; j--) {
      if (this_bit == 8) {
        sprite++;
        this_byte = ~pgm_read_byte_near(sprite);
        this_bit=0;
      }
      
      draw_bit = (this_byte << this_bit) & B10000000;
      
      if (!draw_bit) gamby.setPixel(x+i,y+j,draw_bit);
      this_bit++;
    }
  }
}

void helper_draw_big_char (const prog_uchar *font, char x, char y, char c)
{
    //set the offset to the character
    c -= 48;
    gamby.drawSprite(x, y, font, c);
}

void helper_print_big (const prog_uchar *font, char x, char y, char *s) {
  for (int c=0; s[c] != '\0'; c++)
  {
    helper_draw_big_char(font, x,y, s[c]);
    x +=11;
  }
}

void helper_print_big_number(const prog_uchar *font, char x, char y, unsigned long n, char pad) {
  char buf[10];
  itoa(n, buf, 10);
  char charLen = strlen(buf);
  char padding = pad-charLen;  
  for (char i=0; i< padding; i++)
  {
    helper_draw_big_char(font, x,y, '0');
    x +=11;
  } 
 
  helper_print_big(font, x, y, buf);
}

void helper_draw_char(const prog_uchar *font, char x, char y, char c)
{
    //set the offset to the character
    font += (c - 32) * 2;
	
    byte this_byte;
    byte this_bit=0;
    byte draw_bit = 0;
	
    this_byte = ~pgm_read_byte_near(font);

    for (byte i=0; i<4; i++) {
	
       for (byte j=0; j<4; j++) {
         if (this_bit == 8) {
            font++;
            this_byte = ~pgm_read_byte_near(font);
            this_bit=0;
         }
      
         draw_bit = (this_byte << this_bit) & B10000000;
      
         if (!draw_bit) gamby.setPixel(x+j,y+i,draw_bit);
         this_bit++;
       }
    }
}

void helper_print_p(const prog_uchar *font, char x, char y, const char *s)
{
  char c = pgm_read_byte_near(s);
  while (c != '\0') {
    helper_draw_char(font, x, y, c);
    c = pgm_read_byte_near(++s);
    x +=4;
  }  
}

void helper_print(const prog_uchar *font, char x, char y, char* s) {
  for (int c=0; s[c] != '\0'; c++)
  {
    helper_draw_char(font, x,y, s[c]);
    x +=4;
  }
}

void helper_print_number(const prog_uchar *font, char x, char y, unsigned long n) {
  char buf[10];
  helper_print(font, x, y, itoa(n, buf, 10));
}
