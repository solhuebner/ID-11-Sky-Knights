#ifndef __LEVEL__
#define __LEVEL__

#define STATE_HIDDEN     0
#define STATE_VISIBLE    1
#define STATE_MOVE_LEFT  2
#define STATE_MOVE_RIGHT 4
#define STATE_FALL_LEFT  5
#define STATE_FALL_RIGHT 6

#define EGG_STATE_FALL   7
#define EGG_STATE_STOP   8
#define EGG_STATE_HATCH  9

#define LAVA_STATE_BURN 10

#define TYPE_BIRD_RIDER  0   //Normal enemy
#define TYPE_BIRD        4   //Riderless bird
#define TYPE_LAVA        10  //Lava

#define TYPE_EGG_ENEMY  51  //Egg - does not use the counter (so infinite)
#define TYPE_EGG_KNIGHT 53  //Egg that the knight can hatch from

#define MAX_ELEMENT_RECORDS 16

#define COUNTER_START    25

struct LevelElement
{
  unsigned char id;          //elment identifier
  unsigned char type;        //element type
  unsigned char x;           // y position
  unsigned char y;           // x position
  unsigned char state;       //determines the state of the the element
  unsigned char speed;       //deternimes how fast the object moves
  unsigned char step;        //used to maintain animation step
  unsigned char counter;     //used for timed events
  unsigned char speed_counter;  //used to time the speed of the object
};

void level_reset();
void level_draw();
void level_advance();
void level_update();
char level_check_move(char x, char y);
char level_check_move_full(char x, char y);
char level_get_block(char level_x, char level_y);
char level_check_move_h(char x, char y, char &tile_x);
char level_check_move_v(char x, char y, char &tile_x);
void level_element_add(char type, char x, char y, char state, char speed);
void level_element_handle();
bool level_test_element (LevelElement element, char testX, char testY);
LevelElement level_get_element (char id);
void level_update_element (LevelElement element);
void level_calculate();
#endif __LEVEL__
