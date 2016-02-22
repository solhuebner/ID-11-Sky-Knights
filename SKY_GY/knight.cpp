#include <avr/pgmspace.h>
#include <Gamby.h>
#include "knight.h"
#include "knight_bitmap.h"
#include "level.h"

extern GambyGraphicsMode gamby;

#define MAX_VELOCITY 3
#define STEP_LENGTH 2

char h_velocity = 0;
char y_velocity = 0;
char x = 48;
char y = 39;
char facing = FACING_LEFT;
char frame = 0;
char state = KNIGHTSTATE_PLAYING;
bool flying = false;
bool breaking = false;

char count = 0;

void knight_draw()
{
  char tile_adjust;
  
  if (state == KNIGHTSTATE_PLAYING)
  {
    //(gravity effect) - check space undearneath knight; if it is 0, then knight is falling (flying)
    if (!flying)
    {
      if (level_check_move(x,y + 9) == 0) flying = true;
    } 
    
    //handle movement/animation
    if ((count >= MAX_VELOCITY - h_velocity) && (h_velocity > 0) || (flying))
    {
       count = 0;
    
       if (!breaking)
       {
  
         frame++; 
         if (flying)
         {
  	     if ((facing == FACING_LEFT) && (frame > 6)) frame = KNIGHT_FLY_LEFT;
  	     if ((facing == FACING_RIGHT) && (frame > 13)) frame = KNIGHT_FLY_RIGHT;
  	   } else {
  	     if ((facing == FACING_LEFT) && (frame > 3)) frame = KNIGHT_MOVE_LEFT;
  	     if ((facing == FACING_RIGHT) && (frame > 10)) frame = KNIGHT_MOVE_RIGHT;	  
  	   }
  	
  	   if (facing == FACING_LEFT)
  	   {
  	     if (x > h_velocity)
  	     {
  		  //check to see if we hit anything in the move
  		  if (level_check_move_v(x-h_velocity, y, tile_adjust) == 0) {
  		    //if not, keep on moving
                          x -= h_velocity;
  		  } else {
  		    //hit something, so stop horizontal velocity
                          x = ((tile_adjust + 1) * 8) + 1;
  			h_velocity = 0;
  		  }
  	     } else {
  		  //check to see if we hit anything in the move
  	      if (level_check_move_v(x-h_velocity, y, tile_adjust) == 0) {
                          x = (88 - h_velocity);
  		  } else {
                          x = ((tile_adjust + 1) * 8) + 1;
  			h_velocity = 0;
  		  }
  	     }
         } else {
  	     if (x < (88 -h_velocity))
  	     {
  		  if (level_check_move_v(x+h_velocity, y, tile_adjust) == 0) {
  	         x += h_velocity;
  		  } else {
                      x = (tile_adjust * 8) - 9;
  		    h_velocity = 0;
  		  }
  	     } else {
  		   if (level_check_move_v(0, y, tile_adjust) == 0) {
  	         x = 0;
  		   } else {
                       x = (tile_adjust * 8) - 9;
  		     h_velocity = 0;
  		   }
  	     }
         }
  	  
  	   if (flying)
  	   {
  	     if (y_velocity == 0)
  	     {
  		  if (y < 56)
                  {
  			if (level_check_move_h(x,y + 9, tile_adjust) == 0)
                        {
  			   y++;
  			} else {
                             y = (tile_adjust * 8) - 9;
  			   flying = false;
  			}
  		  } else {
                    //otherwise we have fallen off the map
                    knight_hit();
                  }
            } else {
                 if ((y < 56) && (y - y_velocity > 0)) {
  		     if (level_check_move_h(x, y-y_velocity, tile_adjust) == 0) {
  		        y -= y_velocity;
  		     } else {
                        y = ((tile_adjust + 1) * 8) + 1;
  		        y_velocity = 0;
  		     }
                  }
                  if (y > 56) knight_hit();
            }           
  	   }
  
         if (y_velocity > 0) y_velocity--; 
  	   
       } else{
         breaking = false;
       }
    
    } else {
       count++;
    }
    gamby.drawSprite(x, y, knight_bitmap, frame);
  }
}

void knight_move(char new_facing)
{
  //if we are still moving in the same direction, accelerate
  if (new_facing == facing)
  {
    if (h_velocity <= MAX_VELOCITY) h_velocity++;
  } else {
    //test to see if the knight is at a full stop
    if (h_velocity < 1)
	{
	    facing = new_facing;
	    if (facing == FACING_LEFT)
	    {
	        frame = KNIGHT_MOVE_LEFT;
	    } else {
	        frame = KNIGHT_MOVE_RIGHT;
	    }
	    h_velocity = 1;

	//otherwise, were were in motion so act appropriately  
     } else {
       //if we were going fast enough, start breaking
	 if (!flying) breaking = true;
	 if (facing == FACING_LEFT)
	 {
	  frame = KNIGHT_BREAK_LEFT;
	 } else {
	  frame = KNIGHT_BREAK_RIGHT;
	 }
	 h_velocity--;
      }
  }
}

void knight_init(char newX, char newY)
{
 h_velocity = 0;
 y_velocity = 0;
 x = newX;
 y = newY;
 facing = FACING_LEFT;
 frame = 0;
 state = KNIGHTSTATE_PLAYING;
 flying = false;
 breaking = false;

 count = 0;
}

void knight_fly()
{
  flying = true;
  y_velocity = MAX_VELOCITY;
}

char knight_get_facing()
{
  return facing;
}

char knight_get_y_velocity()
{
  return y_velocity;
}

char knight_get_x()
{
  return x;
}

char knight_get_y()
{
  return y;
}

char knight_get_state()
{
  return state;
}

void knight_hit()
{
  state = KNIGHTSTATE_DEAD;
  
  LevelElement eggElement = level_get_element(0);
                  
  eggElement.state  = EGG_STATE_FALL;
  eggElement.step = 0;
  eggElement.counter = COUNTER_START;
  eggElement.x = x;
  eggElement.y = y;
                   
  level_update_element(eggElement);
}
