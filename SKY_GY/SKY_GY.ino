/*
 Sky Knights: http://www.team-arg.org/sky-manual.html
 
 Gamby version 1.0: http://www.team-arg.org/sky-downloads.html
 
 MADE by TEAM a.r.g. : http://www.team-arg.org/About.html
 
 2013 - Trodoss - JO3RI

 Game License: MIT : https://opensource.org/licenses/MIT
 
 */

//determine the game
#define GAME_ID 11

#include <Gamby.h>
#include <avr/pgmspace.h>

#define PAUSE_BETWEEN_ACTIONS 100
#define PAUSE_BETWEEN_MENUS   125

//define menu states (on main menu)
#define STATE_MENU_MAIN    0
#define STATE_MENU_HELP    1
#define STATE_MENU_CREDITS 2
#define STATE_MENU_SOUND   3

#define GAME_PLAYING 4
#define GAME_PAUSED  5
#define GAME_WON     6
#define GAME_OVER    7
#define GAME_OVER_START 8
#define GAME_NEXT_LEVEL_START 9
#define GAME_NEXT_LEVEL 10

#include "other_bitmap.h"
#include "text.h"
#include "font.h"

#include "level.h"
#include "knight.h"
#include "sound.h"
#include "helper.h"

GambyGraphicsMode gamby;

//determines the state of the game
unsigned char game_state = 0;
static long next_action = 0;
unsigned long score;
char life, enemies, eggs;

//generic variable to store selection (on screens)
unsigned char selection = 0;
boolean soundyesno = true;

void setup()
{
  gamby.drawPattern = PATTERN_BLACK;
  gamby.rect(0,0,95,63);
  gamby.font = font;
  gamby.drawSprite(2,16,TEAM_arg_logo);
  gamby.update();
  delay(3000L);

  randomSeed(analogRead(0));
  score = 0;
  life = 2;

  game_state = STATE_MENU_MAIN;
  sound_play_song_once(0);
}

void loop()
{  
  if (soundyesno == true) sound_update();
  byte lastInputs;
  switch (game_state)
  {

    //******* MENU-specific events **************    

  case STATE_MENU_MAIN:
    menu_main();
    if (millis() >= next_action)
    {
      gamby.readInputs();
      lastInputs = gamby.inputs;             

      if (gamby.inputs & DPAD_UP)
      {
        if (selection > 0) selection--;
      }

      if (gamby.inputs & DPAD_DOWN)
      {
        if (selection < 3) selection++;
      }
      if (gamby.inputs & (BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4))
      {
        gamby.clearScreen();
        gamby.update();

        switch (selection)
        {
        case 0:
          game_state = STATE_MENU_HELP;
          menu_help();
          break;

        case 1:
          game_state = STATE_MENU_CREDITS;
          menu_credits();
          break;

        case 2:
          game_state = STATE_MENU_SOUND;
          break;

        case 3:
          game_state = GAME_PLAYING;
          sound_stop();
          start_game();
          break;
        }
      }

      next_action = millis() + PAUSE_BETWEEN_MENUS; 
    }
    break;

  case STATE_MENU_HELP:
  case STATE_MENU_CREDITS:
    if (millis() >= next_action)
    {
      gamby.readInputs();
      lastInputs = gamby.inputs;             

      if (gamby.inputs & (BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4))
      {
        gamby.clearScreen();
        gamby.update();
        game_state = STATE_MENU_MAIN;
      }
      next_action = millis() + PAUSE_BETWEEN_MENUS; 
    }
    break;

  case STATE_MENU_SOUND:
    menu_sound();
    if (millis() >= next_action)
    {
      gamby.readInputs();
      lastInputs = gamby.inputs;             
      if (gamby.inputs & DPAD_UP) {
        if (soundyesno == false) soundyesno = true;
      }
      if (gamby.inputs & DPAD_DOWN) {
        if (soundyesno == true) soundyesno = false;
      }
      if (gamby.inputs & (BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4))
      {
        sound_toggle(soundyesno);
        game_state = STATE_MENU_MAIN; 
      }
      next_action = millis() + PAUSE_BETWEEN_MENUS; 
    }
    break;   


    //******* Game-specific events **************

  case GAME_OVER_START:
    start_game_over();
    break;

  case GAME_NEXT_LEVEL_START:
    start_next_level();
    break;	   

  case GAME_OVER:
  case GAME_WON:
    if (millis() >= next_action)
    {
      gamby.readInputs();
      lastInputs = gamby.inputs;
      //wait for a button to be pressed to continue
      if (lastInputs & (BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4))
      {

        //check to see what we are doing when we start the game
        if (game_state > STATE_MENU_MAIN)
        {
          if (game_state == GAME_WON)
          {
            //to do: win game
          } 
          else {
            //to do: handle returning to the title
          }
        }
        game_state = STATE_MENU_MAIN;
        clear_screen();
        score = 0;
        life = 2;
        level_reset();
        sound_play_song_once(0);
      }

      next_action = millis() + PAUSE_BETWEEN_MENUS; 
    }	
    break; 

  case GAME_PLAYING:
    if (millis() >= next_action)
    { 
      gamby.readInputs();     
      lastInputs = gamby.inputs;

      if (lastInputs & DPAD_RIGHT) knight_move(FACING_RIGHT);
      if (lastInputs & DPAD_LEFT) knight_move(FACING_LEFT);
      if (lastInputs & (BUTTON_2 | BUTTON_4)) knight_fly();
      if (lastInputs & BUTTON_1) game_state = GAME_PAUSED;

      next_action = millis() + PAUSE_BETWEEN_ACTIONS; 

      level_update();
    }
    gamby.update();
    break;

  case GAME_PAUSED:
    if (millis() >= next_action)
    {
      gamby.readInputs();
      lastInputs = gamby.inputs;

      if (lastInputs & (BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4)) game_state = GAME_PLAYING;

      next_action = millis() + PAUSE_BETWEEN_MENUS; 
    }
    break;

  case GAME_NEXT_LEVEL: 
    if (millis() >= next_action)
    {
      gamby.readInputs();
      lastInputs = gamby.inputs;

      if (lastInputs & (BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4))
      {
        level_advance();
        game_state = GAME_PLAYING;
      }
      next_action = millis() + PAUSE_BETWEEN_MENUS; 
    }
    break;
  }  
}


//****************** Menu-specific functions
void menu_main()
{  
  // clean up the screen
  clear_screen();

  // show the splash art
  gamby.drawSprite(37,-2,sky,sky);
  gamby.drawSprite(23,13,knights,knights);
  gamby.drawSprite(0,-1,knightleft,knightleft);
  gamby.drawSprite(69,-1,knightright,knightright);

  // show the menu
  gamby.drawSprite(37,29,menucenter,menucenter);
  gamby.drawSprite(32,30+(8*selection),selectorleft);
  gamby.drawSprite(60,30+(8*selection),selectorright);
  gamby.update();
}


void menu_credits()
{
  clear_screen();

  gamby.drawSprite(9,-2,sky);
  gamby.drawSprite(37,-2,knights);
  gamby.setPos (0,3);
  gamby.print_P(Credits);
  gamby.update();  
}

void menu_help()
{
  clear_screen();
  gamby.drawSprite(19,-4,QRcode);
  gamby.update();  
}

void menu_sound()
{
  clear_screen();
  switch (soundyesno){
  case true:
    gamby.drawSprite(31,7,soundFX);
    break;
  case false:
    gamby.drawSprite(31,7,nosoundFX);   
    break;
  }
  gamby.drawSprite(42,46,textyesno);
  gamby.drawSprite(32,55-(8*soundyesno),selectorleft);
  gamby.drawSprite(60,55-(8*soundyesno),selectorright);
  gamby.update();  
}

void start_game()
{
  game_state = GAME_PLAYING;  
  score = 0;
  life = 2;
  level_advance();
}

void start_game_over()
{
  game_state = GAME_OVER;
  clear_screen();
  gamby.drawSprite(0,-2,gameover_bitmap);
  gamby.drawSprite(0,15,over_bitmap);
  gamby.update();
  delay (2000);
  clear_screen();
  gamby.drawSprite(0,18,your_score_bitmap);
  helper_print_big_number(numbers_bitmap, 6, 36, score, 8);
  gamby.update();
  gamby.inputs = 0;
  gamby.update();
}

void start_next_level()
{
  sound_stop();
  game_state = GAME_NEXT_LEVEL;
  clear_screen();
  gamby.drawSprite(0,0,your_score_bitmap);
  gamby.drawSprite(48,18,roast_bitmap);
  gamby.drawSprite(4,18,roast_bitmap);
  helper_print_big_number(numbers_bitmap, 6, 45, score, 8);
  gamby.update();
  delay (2000);
  clear_screen();
  level_calculate();
  gamby.drawSprite(0,-2,next_level_bitmap);
  gamby.drawSprite(24,14,knight2x_bitmap);
  gamby.drawSprite(24,31,rider2x_bitmap);
  gamby.drawSprite(24,48,egg2x_bitmap);
  helper_print_big_number(numbers_bitmap, 50, 14, life+1 ,3);
  helper_print_big_number(numbers_bitmap, 50, 31, enemies,3);
  helper_print_big_number(numbers_bitmap, 50, 47, eggs,3);
  gamby.inputs = 0;
  gamby.update();
  sound_play_song_once(2);
  
}

void start_game_won()
{
}

void clear_screen()
{  
  gamby.drawPattern = PATTERN_WHITE;
  gamby.rect(0,0,95,63);
  gamby.drawPattern = PATTERN_BLACK;
}



