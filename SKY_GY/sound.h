#ifndef __SOUND__
#define __SOUND__

#include <avr/pgmspace.h>

void sound_play_song(char song);
void sound_play_song_once(char song);
void sound_update();
void sound_play_sfx(char song);
void sound_stop();
void sound_toggle(boolean toggle);

#endif __SOUND__
