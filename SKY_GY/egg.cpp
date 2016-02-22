#include <avr/pgmspace.h>
#include <Gamby.h>
#include "egg.h"
#include "egg_bitmap.h"
#include "level.h"
#include "knight.h"

extern GambyGraphicsMode gamby;
extern long score;

#define STEP_LENGTH  2

LevelElement egg_move(LevelElement element)
{
  char adjust;
  
  //draw new monster bitmap
  if (element.state > STATE_HIDDEN)
  {
  
    if (element.speed_counter == element.speed)
    {
        element.speed_counter = 0;
       
        switch (element.state)
        {
    	 
    	  case EGG_STATE_FALL:
               element.state = EGG_STATE_FALL;
               //check above; if it is possible to move down, then do so
               if (level_check_move_h(element.x, element.y +8 +STEP_LENGTH, adjust) == 0)
               { 
                 if ((element.y + 8 + STEP_LENGTH) < 56)
                 {
                   element.y += STEP_LENGTH;
                 } else {
                   //fallen past the normal ledge, so we will place
                   //the egg on the platform
                   element.x = 40;
                   element.y = 47;
                   element.state = EGG_STATE_STOP;
                 }
               } else {
                 element.y = (adjust * 8) - 9;
                 element.state = EGG_STATE_STOP;
               }
               break;	   
    	   
    	  case EGG_STATE_STOP:
               element.state = EGG_STATE_STOP;
			   if (element.counter > 0)
			   {
			      element.counter--;
			   } else {
			      element.state = EGG_STATE_HATCH;
			   }
               break;
			   
    	  case EGG_STATE_HATCH:
               element.state = EGG_STATE_HATCH;
			   element.step++;
               if (element.step > 2)
			   {
			      element.state = STATE_HIDDEN;
			      if (element.type == TYPE_EGG_ENEMY)
				  {
				      level_element_add(TYPE_BIRD_RIDER, element.x, element.y, STATE_MOVE_LEFT, 1);
				  } else {
				      knight_init(element.x, element.y);
				  }
			   }
    	   break;		      
        }
       }
       else
       {
         element.speed_counter++;
       }
       gamby.drawSprite(element.x, element.y, egg_bitmap, element.step); 
  }
  return element;
}

LevelElement egg_hit(LevelElement element)
{
   
  element.state = STATE_HIDDEN;
   
  return element;
}



