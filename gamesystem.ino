#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include <EEPROM.h>  
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define PRIMARY_COLOR 0x4A11
#define PRIMARY_LIGHT_COLOR 0x7A17
#define PRIMARY_DARK_COLOR 0x4016
#define PRIMARY_TEXT_COLOR 0x7FFF
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define LOWFLASH (defined(__AVR_ATmega328P__) && defined(MCUFRIEND_KBV_H_))
#define SENSIBILITY 50
#define MINPRESSURE 1
#define MAXPRESSURE 1000
#define YP A3
#define XM A2 
#define YM 9  
#define XP 8 
#define TS_MINX 120
#define TS_MINY 70
#define TS_MAXX 900
#define TS_MAXY 920
#define BOXSIZE 33.4
#define PENRADIUS 1
int oldcolor, currentcolor;
const int16_t TS_LEFT = 122, TS_RT = 929, TS_TOP = 77, TS_BOT = 884;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);
int wing;
int fx, fy, fallRate;
int pillarPos, gapPos;
int score;
int fbutton_id = 0;
bool running = false;
bool crashed = false;
bool scrPress = false;
bool flappy = false;
bool drawing = false;
bool breakout = false;
bool dice = false;
bool menu = true;
long nextDrawLoopRunTime;
#define SCORE_SIZE 30
char scoreFormat[] = "%04d";
typedef struct gameSize_type {
  int16_t x, y, width, height;
} gameSize_type;
gameSize_type gameSize;
uint16_t backgroundColor = BLACK;
int level;
const uint8_t BIT_MASK[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
uint8_t pointsForRow[] = {7, 7, 5, 5, 3, 3 , 1, 1};
#define GAMES_NUMBER 16
typedef struct game_type {
  int ballsize;
  int playerwidth;
  int playerheight;
  int exponent;
  int top;
  int rows;
  int columns;
  int brickGap;
  int lives;
  int wall[GAMES_NUMBER];
  int initVelx;
  int initVely;
} game_type;
game_type games[GAMES_NUMBER] = {
  { 10,             60,          8,           6,        40 ,     8,       8, 3,       3,  {0x18, 0x66, 0xFF, 0xDB, 0xFF, 0x7E, 0x24, 0x3C} , 28, -28},
  { 10,             50,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0x99, 0xFF, 0xE7, 0xBD, 0xDB, 0xE7, 0xFF} , 28, -28},
  { 10,             50,          8,           6,        40 ,     8,       8, 3,       3,  {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55} , 28, -28},
  { 8,              50,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF} , 34, -34},
  { 10,             40,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0xAA, 0xAA, 0xFF, 0xFF, 0xAA, 0xAA, 0xFF} , 28, -28},
  { 10,             40,          8,           6,        40 ,     8,       8, 3,       3,  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} , 28, -28},
  { 12,             64,          8,           6,        60 ,     4,       2, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 20, -20},
  { 12,             60,          8,           6,        60 ,     5,       3, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 22, -22},
  { 10,             56,          8,           6,        30 ,     6,       4, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 24, -24},
  { 10,             52,          8,           6,        30 ,     7,       5, 3,       4,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 26, -26},
  { 8,              48,          8,           6,        30 ,     8,       6, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 28, -28},
  { 8,              44,          8,           6,        30 ,     8,       7, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 30, -30},
  { 8,              40,          8,           6,        30 ,     8,       8, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 32, -32},
  { 8,              36,          8,           6,        40 ,     8,       8, 3,       3,  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , 34, -34},
  { 8,              36,          8,           6,        40 ,     8,       8, 3,       3,  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} , 34, -34}
};
game_type* game;
typedef struct game_state_type {
  uint16_t ballx;
  uint16_t bally;
  uint16_t ballxold;
  uint16_t ballyold;
  int velx;
  int vely;
  int playerx;
  int playerxold;
  int wallState[8];
  int score;
  int remainingLives;
  int top;
  int bottom;
  int walltop;
  int wallbottom ;
  int brickheight;
  int brickwidth;
};
int lopforx;
int lopit;
int randNumber;
int activate = 0;
game_state_type state;
void setup() {
  Serial.begin(9600);
  gameSize = {0, 0, tft.width(), tft.height()};
  randomSeed(analogRead(5));
  tft.reset();
  tft.begin(0x9341); 
  tft.setRotation(3); 
  tft.fillScreen(BLACK);
  tft.fillRect(0,0,160,120,YELLOW);
  tft.setCursor (20, 30);
  tft.setTextSize (3);
  tft.setTextColor(GREEN);
  tft.println("Flappy");
  tft.setCursor (30, 70);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("Score:");
  tft.setCursor (105, 70);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println(EEPROM.read(0));
  tft.setCursor (50, 100);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Start");
  tft.fillRect(160,0,160,120,MAGENTA);
  tft.setCursor (200, 30);
  tft.setTextSize (3);
  tft.setTextColor(CYAN);
  tft.println("Dice");
  tft.setCursor (210, 85);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Start");
  tft.fillRect(0,120,160,120,GREEN);
  tft.setCursor (20, 145);
  tft.setTextSize (3);
  tft.setTextColor(BLUE);
  tft.println("Drawing");
  tft.setCursor (50, 200);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Start");
  tft.fillRect(160,120,160,120,BLUE);
  tft.setCursor (170, 145);
  tft.setTextSize (3);
  tft.setTextColor(WHITE);
  tft.println("Breakout");
  tft.setCursor (210, 200);
  tft.setTextSize (2);
  tft.setTextColor(RED);
  tft.println("Start");
}
void(* resetFunc) (void) = 0;
void drawinggame(){
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
  tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, BLACK);
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
  currentcolor = RED;
  pinMode(13, OUTPUT);
}
int selection = -1;
void loop(void) {
  if(menu == true){
    TSPoint p = ts.getPoint();
    if (p.z > ts.pressureThreshhold) {
     p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
     p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
     if(p.x < tft.width() / 2 && p.y > tft.height() / 2) {
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
      tft.fillScreen(GREEN);
      flappy = false;
      drawing = true;
      breakout = false;
      menu = false;
      dice = false;
      drawinggame();
     }
     if(p.x < tft.width() / 2 && p.y < tft.height() / 2) {
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
      tft.fillScreen(BLACK);
      flappy = false;
      drawing = false;
      breakout = true;
      menu = false;
      dice = false;
      tft.setRotation(4);
      tft.setCursor (100, 10);
      newGame(&games[0], &state, tft);
     }
     if(p.x > tft.width() / 2 && p.y < tft.height() / 2) {
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
      tft.fillScreen(YELLOW);
      flappy = false;
      drawing = false;
      breakout = false;
      menu = false;
      dice = true;
      tft.setRotation(4);
      roledice();
     }
     if(p.x > tft.width() / 2 && p.y > tft.height() / 2){
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
      tft.fillScreen(BLUE);
      flappy = true;
      drawing = false;
      breakout = false;
      menu = false;
      dice = false;
      startGame();
     }
    }    
  }else{
  if(flappy == true){
     tft.setCursor(300, 10);
     tft.setTextColor(WHITE);
     tft.print(score);
     if (millis() > nextDrawLoopRunTime ) {
        drawLoop();
          checkCollision();
        nextDrawLoopRunTime += 100;
    }
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      if (!running) {
        tft.fillRect(0,0,320,80,BLUE);
        running = true;
      }else{
        fallRate = -6;
        scrPress = true;
      }
    }else if (p.z == 0 && scrPress) {
      scrPress = false;
    }
  }else{
    if(drawing == true){
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    if (p.y < (TS_MINY-5)) {
      Serial.println("erase");
      tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, BLACK);
    }
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    if (p.y < BOXSIZE) {
       oldcolor = currentcolor;
       if (p.x < BOXSIZE) { 
         currentcolor = RED; 
         tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*2) {
         currentcolor = YELLOW;
         tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*3) {
         currentcolor = GREEN;
         tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*4) {
         currentcolor = CYAN;
         tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*5) {
         currentcolor = BLUE;
         tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*6) {
         currentcolor = MAGENTA;
         tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
       } else if (p.x < BOXSIZE*7) {
         resetFunc();
       }
       if (oldcolor != currentcolor) {
          if (oldcolor == RED) tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
          if (oldcolor == YELLOW) tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
          if (oldcolor == GREEN) tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
          if (oldcolor == CYAN) tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
          if (oldcolor == BLUE) tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
          if (oldcolor == MAGENTA) tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
       }
    }
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
   }
  }else{
   if(breakout == true){
     selection = readUiSelection(game, &state, selection);
  drawPlayer(game, &state);
  state.playerxold = state.playerx;
  if (abs( state.vely) > ((1 << game->exponent) - 1)) {
    state.vely = ((1 << game->exponent) - 1) * ((state.vely > 0) - (state.vely < 0));
  }
  if (abs( state.velx) > ((1 << game->exponent) - 1)) {
    state.velx = ((1 << game->exponent) - 1) * ((state.velx > 0) - (state.velx < 0));
  }
  state.ballx += state.velx;
  state.bally += state.vely;
  checkBallCollisions(game, &state, state.ballx >> game->exponent, state.bally >> game->exponent);
  checkBallExit(game, &state, state.ballx >> game->exponent, state.bally >> game->exponent);
  drawBall(state.ballx >> game->exponent, state.bally >> game->exponent, state.ballxold >> game->exponent, state.ballyold >> game->exponent, game->ballsize );
  state.ballxold = state.ballx;
  state.ballyold = state.bally;
  state.velx = (20 + (state.score >> 3 )) * ( (state.velx > 0) - (state.velx < 0));
  state.vely = (20 + (state.score >> 3 )) * ( (state.vely > 0) - (state.vely < 0));
  if (noBricks(game, &state) && level < GAMES_NUMBER) {
    level++;
    newGame( &games[level], &state, tft);
  } else if ( state.remainingLives <= 0) {
    gameOverTouchToStart();
    state.score = 0;
    level = 0;
    newGame(game, &state, tft);
  }
   }else{
    if(dice == true){
      int16_t xpos, ypos; 
      TSPoint tp = ts.getPoint();  
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
        xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
        ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
        if (ypos > 280) {
          resetFunc();
        }else{
          roledice();
        }
      }
    }
   }
  }}}
}
TSPoint waitOneTouch(boolean showMessage) {
  uint8_t save = 0;
  if (showMessage) {
    save = tft.getRotation(); 
    tft.setRotation(3); 
    tft.setCursor (180, 200);
    tft.setTextSize (1);
    tft.setTextColor(WHITE);
    tft.println("Click to restart");
  }
  TSPoint p;
  do {
    p= ts.getPoint(); 
    pinMode(XM, OUTPUT); 
    pinMode(YP, OUTPUT);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  if (showMessage) {
    tft.setRotation(save);
  }
  return p;
}
void drawBorder () {
  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;
  tft.fillScreen(BLUE);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
}
void drawFlappy(int x, int y) {
  tft.fillRect(x+2, y+8, 2, 10, BLACK);
  tft.fillRect(x+4, y+6, 2, 2, BLACK);
  tft.fillRect(x+6, y+4, 2, 2, BLACK);
  tft.fillRect(x+8, y+2, 4, 2, BLACK);
  tft.fillRect(x+12, y, 12, 2, BLACK);
  tft.fillRect(x+24, y+2, 2, 2, BLACK);
  tft.fillRect(x+26, y+4, 2, 2, BLACK);
  tft.fillRect(x+28, y+6, 2, 6, BLACK);
  tft.fillRect(x+10, y+22, 10, 2, BLACK);
  tft.fillRect(x+4, y+18, 2, 2, BLACK);
  tft.fillRect(x+6, y+20, 4, 2, BLACK);
  tft.fillRect(x+12, y+2, 6, 2, YELLOW);
  tft.fillRect(x+8, y+4, 8, 2, YELLOW);
  tft.fillRect(x+6, y+6, 10, 2, YELLOW);
  tft.fillRect(x+4, y+8, 12, 2, YELLOW);
  tft.fillRect(x+4, y+10, 14, 2, YELLOW);
  tft.fillRect(x+4, y+12, 16, 2, YELLOW);
  tft.fillRect(x+4, y+14, 14, 2, YELLOW);
  tft.fillRect(x+4, y+16, 12, 2, YELLOW);
  tft.fillRect(x+6, y+18, 12, 2, YELLOW);
  tft.fillRect(x+10, y+20, 10, 2, YELLOW);
  tft.fillRect(x+18, y+2, 2, 2, BLACK);
  tft.fillRect(x+16, y+4, 2, 6, BLACK);
  tft.fillRect(x+18, y+10, 2, 2, BLACK);
  tft.fillRect(x+18, y+4, 2, 6, WHITE);
  tft.fillRect(x+20, y+2, 4, 10, WHITE);
  tft.fillRect(x+24, y+4, 2, 8, WHITE);
  tft.fillRect(x+26, y+6, 2, 6, WHITE);
  tft.fillRect(x+24, y+6, 2, 4, BLACK);
  tft.fillRect(x+20, y+12, 12, 2, BLACK);
  tft.fillRect(x+18, y+14, 2, 2, BLACK);
  tft.fillRect(x+20, y+14, 12, 2, RED);
  tft.fillRect(x+32, y+14, 2, 2, BLACK);
  tft.fillRect(x+16, y+16, 2, 2, BLACK);
  tft.fillRect(x+18, y+16, 2, 2, RED);
  tft.fillRect(x+20, y+16, 12, 2, BLACK);
  tft.fillRect(x+18, y+18, 2, 2, BLACK);
  tft.fillRect(x+20, y+18, 10, 2, RED);
  tft.fillRect(x+30, y+18, 2, 2, BLACK);
  tft.fillRect(x+20, y+20, 10, 2, BLACK);
}
void startGame() {
  int fx=50;
  int fy=125;
  int fallRate=1;
  int pillarPos = 320;
  int gapPos = 60;
  bool crashed = false;
  int score = 0;
  tft.fillScreen(BLUE); 
  int ty = 230;
  for (int tx = 0; tx <= 300; tx +=20) {
    tft.fillTriangle(tx,ty, tx+10,ty, tx,ty+10, GREEN);
    tft.fillTriangle(tx+10,ty+10, tx+10,ty, tx,ty+10, YELLOW);
    tft.fillTriangle(tx+10,ty, tx+20,ty, tx+10,ty+10, YELLOW);
    tft.fillTriangle(tx+20,ty+10, tx+20,ty, tx+10,ty+10, GREEN);
  }
}
void drawPillar(int x, int gap) {
  tft.fillRect(x+2, 2, 46, gap-4, GREEN);
  tft.fillRect(x+2, gap+92, 46, 136-gap, GREEN);
  tft.drawRect(x,0,50,gap,BLACK);
  tft.drawRect(x+1,1,48,gap-2,BLACK);
  tft.drawRect(x, gap+90, 50, 140-gap, BLACK);
  tft.drawRect(x+1,gap+91 ,48, 138-gap, BLACK);
}
void clearPillar(int x, int gap) {
  tft.fillRect(x+45, 0, 5, gap, BLUE);
  tft.fillRect(x+45, gap+90, 5, 140-gap, BLUE);
}
void clearFlappy(int x, int y) {
 tft.fillRect(x, y, 34, 24, BLUE); 
}
void checkCollision() {
  if (fy > 206) crashed = true;
  if (fx + 34 > pillarPos && fx < pillarPos + 50)
    if (fy < gapPos || fy + 24 > gapPos + 90)
      crashed = true; 
  if (crashed) {
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(75, 75);
    tft.print("Game Over!");
    int highscore = EEPROM.read(0);
    if(score > highscore){
       EEPROM.update(0, score);
       tft.setCursor(75, 125);
       tft.print("Highscore: ");
       tft.setCursor(220, 125);
       tft.print(score);
    }else{
       tft.setCursor(75, 125);
       tft.print("Score:");
       tft.setCursor(220, 125);
       tft.print(score);
    }
    running = false;
    waitOneTouch(true);
    resetFunc();
  }
}
void drawLoop() {
  clearPillar(pillarPos, gapPos);
  clearFlappy(fx, fy);
  if (running) {
    fy += fallRate;
    fallRate++;
    pillarPos -=5;
    if (pillarPos == 0) {
      score++;
    }
    else if (pillarPos < -50) {
      pillarPos = 320;
      gapPos = random(20, 120);
    }
  }
  drawPillar(pillarPos, gapPos);
  drawFlappy(fx, fy);
}
void newGame(game_type* newGame, game_state_type * state,  Adafruit_TFTLCD &tft) {
  game = newGame;
  setupState(game, state, tft);
  clearDialog(gameSize);
  updateLives(game->lives, state->remainingLives);
  updateScore(state->score);
  setupWall(game, state);
  touchToStart();
  clearDialog(gameSize);
  updateLives(game->lives, state->remainingLives);
  updateScore(state->score);
  setupWall(game, state);
}
void setupStateSizes(game_type* game, game_state_type * state, Adafruit_TFTLCD &tft) {
  state->bottom = tft.height() - 30;
  state->brickwidth = tft.width() / game->columns;
  state->brickheight = tft.height() / 24;
}
void setupState(game_type* game, game_state_type * state, Adafruit_TFTLCD &tft) {
  setupStateSizes(game, state, tft);
  for (int i = 0; i < game->rows ; i ++) {
    state->wallState[i] = 0;
  }
  state->playerx = tft.width() / 2 - game->playerwidth / 2;
  state->remainingLives = game->lives;
  state->bally = state->bottom << game->exponent;
  state->ballyold = state->bottom << game->exponent;
  state->velx = game->initVelx;
  state->vely = game->initVely;
}
void updateLives(int lives, int remainingLives) {
  for (int i = 0; i < lives; i++) {
    tft.fillCircle((1 + i) * 15, 15, 5, BLACK);
  }
  for (int i = 0; i < remainingLives; i++) {
    tft.fillCircle((1 + i) * 15, 15, 5, YELLOW);
  }
}
void setupWall(game_type * game, game_state_type * state) {
  int colors[] = {RED, RED, BLUE, BLUE,  YELLOW, YELLOW, GREEN, GREEN};
  state->walltop = game->top + 40;
  state->wallbottom = state->walltop + game->rows * state->brickheight;
  for (int i = 0; i < game->rows; i++) {
    for (int j = 0; j < game->columns; j++) {
      if (isBrickIn(game->wall, j, i)) {
        setBrick(state->wallState, j, i);
        drawBrick(state, j, i, colors[i]);
      }
    }
  }
}
void drawBrick(game_state_type * state, int xBrick, int yBrickRow, uint16_t backgroundColor) {
  tft.fillRect((state->brickwidth * xBrick) + game->brickGap,
               state->walltop + (state->brickheight * yBrickRow) + game->brickGap ,
               state->brickwidth - game->brickGap * 2,
               state->brickheight -  game->brickGap * 2, backgroundColor);

}
boolean noBricks(game_type * game, game_state_type * state) {
  for (int i = 0; i < game->rows ; i++) {
    if (state->wallState[i]) return false;
  }
  return true;
}
void drawPlayer(game_type * game, game_state_type * state) {
  tft.fillRect(state->playerx, state->bottom, game->playerwidth, game->playerheight, YELLOW);
  if (state->playerx != state->playerxold) {
    if (state->playerx < state->playerxold) {
      tft.fillRect(state->playerx + game->playerwidth, state->bottom, abs(state->playerx - state->playerxold), game->playerheight, backgroundColor);
    }
    else {
      tft.fillRect(state->playerxold, state->bottom, abs(state->playerx - state->playerxold), game->playerheight, backgroundColor);
    }

  }
}
void drawBall(int x, int y, int xold, int yold, int ballsize) {
  if (xold <= x && yold <= y) {
    tft.fillRect(xold , yold, ballsize, y - yold, BLACK);
    tft.fillRect(xold , yold, x - xold, ballsize, BLACK);
  } else if (xold >= x && yold >= y) {
    tft.fillRect(x + ballsize , yold, xold - x, ballsize, BLACK);
    tft.fillRect(xold , y + ballsize, ballsize, yold - y, BLACK);
  } else if (xold <= x && yold >= y) {
    tft.fillRect(xold , yold, x - xold, ballsize, BLACK);
    tft.fillRect(xold , y + ballsize, ballsize, yold - y, BLACK);
  } else if (xold >= x && yold <= y) {
    tft.fillRect(xold , yold, ballsize, y - yold, BLACK);
    tft.fillRect(x + ballsize, yold, xold - x, ballsize, BLACK);
  }
  tft.fillRect(x , y, ballsize, ballsize, YELLOW);
}
void touchToStart() {
  drawBoxedString(0, 200, "   BREAKOUT", 3, YELLOW, BLACK);
  drawBoxedString(0, 240, "   TOUCH TO START", 2, RED, BLACK);
  while (waitForTouch() < 0) {}
}
void gameOverTouchToStart() {
  drawBoxedString(0, 180, "  GAME OVER", 3, YELLOW, BLACK);
  drawBoxedString(0, 220, "   TOUCH TO HOME", 2, RED, BLACK);
  while (waitForTouch() < 0) {}
}
void updateScore (int score) {
  char buffer[5];
  snprintf(buffer, sizeof(buffer), scoreFormat, score);
  drawBoxedString(tft.width() - 50, 6, buffer, 2, YELLOW, PRIMARY_DARK_COLOR);
}
void checkBrickCollision(game_type* game, game_state_type * state, uint16_t x, uint16_t y) {
  int x1 = x + game->ballsize;
  int y1 = y + game->ballsize;
  int collissions = 0;
  collissions += checkCornerCollision(game, state, x, y);
  collissions += checkCornerCollision(game, state, x1, y1);
  collissions += checkCornerCollision(game, state, x, y1);
  collissions += checkCornerCollision(game, state, x1, y);
  if (collissions > 0 ) {
    state->vely = (-1 * state->vely);
    if ((((x % state->brickwidth) == 0)  && ( state->velx < 0 ))
        || ((((x + game->ballsize) % state->brickwidth) == 0)  && ( state->velx > 0 )) ) {
      state->velx = (-1 * state->velx);
    }
  }
}
int checkCornerCollision(game_type * game,  game_state_type * state, uint16_t x, uint16_t y) {
  if ((y > state->walltop) && (y < state->wallbottom)) {
    int yBrickRow = ( y -  state->walltop) / state->brickheight;
    int xBrickColumn = (x / state->brickwidth);
    if (isBrickIn(state->wallState, xBrickColumn, yBrickRow) ) {
      hitBrick(state, xBrickColumn, yBrickRow);
      return 1;
    }
  }
  return 0;
}
void hitBrick(game_state_type * state, int xBrick, int yBrickRow) {
  state->score += pointsForRow[yBrickRow];
  drawBrick(state, xBrick, yBrickRow, WHITE);
  delay(16);
  drawBrick(state, xBrick, yBrickRow, BLUE);
  delay(8);
  drawBrick(state, xBrick, yBrickRow, backgroundColor);
  unsetBrick(state->wallState, xBrick, yBrickRow);
  updateScore(state->score);
}
void checkBorderCollision(game_type * game,  game_state_type * state, uint16_t x, uint16_t y) {
  if (x + game->ballsize >=  tft.width()) {
    state->velx = -abs(state->velx);
  }
  if (x <= 0  ) {
    state->velx = abs(state->velx);
  }
  if (y <= SCORE_SIZE ) {
    state->vely = abs(state->vely);
  }
  if (((y + game->ballsize)  >=  state->bottom)
      && ((y + game->ballsize) <= (state->bottom + game->playerheight))
      && (x >= state->playerx)
      && (x <= (state->playerx + game->playerwidth))) {
    if (x > (state->playerx + game->playerwidth - 6)) {
      state->velx = state->velx - 1;
    } else if (x < state->playerx + 6) {
      state->velx = state->velx + 1;
    }
    state->vely = -abs(state->vely) ;
  }
}
void checkBallCollisions(game_type * game, game_state_type * state, uint16_t x, uint16_t y) {
  checkBrickCollision(game, state, x, y);
  checkBorderCollision(game, state, x, y);
}
void checkBallExit(game_type * game, game_state_type * state, uint16_t x, uint16_t y) {
  if (((y + game->ballsize)  >=  tft.height())) {
    state->remainingLives--;
    updateLives(game->lives, state->remainingLives);
    delay(500);
    state->vely = -abs(state->vely) ;
  }
}
void setBrick(int wall[], uint8_t x, uint8_t y) {
  wall[y] = wall[y] | BIT_MASK[x];
}
void unsetBrick(int wall[], uint8_t x, uint8_t y) {
  wall[y] = wall[y] & ~BIT_MASK[x];
}
boolean isBrickIn(int wall[], uint8_t x, uint8_t y) {
  return wall[y] &  BIT_MASK[x];
}
void initTft(Adafruit_TFTLCD & tft) {
  tft.reset();
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(0);
}
void drawBoxedString(const uint16_t x, const uint16_t y, const char* string, const uint16_t fontsize, const uint16_t foreColor, const uint16_t backgroundColor) {
  tft.setTextSize(fontsize);
  int16_t  x1, y1;
  uint16_t w, h;
  tft.getTextBounds(string, x, y, &x1, &y1, &w, &h);
  tft.fillRect(x, y, w, h, backgroundColor);
  tft.setCursor(x, y);
  tft.setTextColor(foreColor);
  tft.print(string);
}
void clearDialog(gameSize_type gameSize) {
  tft.fillRect(gameSize.x, gameSize.y, gameSize.width, gameSize.height,  backgroundColor);
  tft.fillRect(gameSize.x, gameSize.y, gameSize.width, SCORE_SIZE, PRIMARY_DARK_COLOR);
}
int readUiSelection(game_type * game, game_state_type * state, const int16_t lastSelected ) {
  int16_t xpos, ypos; 
  TSPoint tp = ts.getPoint();  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
    xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
    ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
    if (ypos < 20) {
      resetFunc();
    }else{
      if (xpos < tft.width() / 2) {
        state->playerx += 4;
      } else {
        state->playerx -= 4;
      }
    }
    if (state->playerx >= tft.width() - game->playerwidth) state->playerx = tft.width() - game->playerwidth;
    if (state->playerx < 0) state->playerx = 0;
    return 1;
  }
#ifdef DEMO_MODE
  state->playerx = (state->ballx >> game->exponent) - game->playerwidth / 2;
  if (state->playerx >= tft.width() - game->playerwidth) state->playerx = tft.width() - game->playerwidth;
  if (state->playerx < 0) state->playerx = 0;
#endif
  return -1;
}
int waitForTouch() {
  int16_t xpos, ypos; 
  TSPoint tp = ts.getPoint();  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
    tft.setRotation(4); 
    return 1;
  }
  return -1;
}
void roledice() {
  lopforx = random(3, 11);
  lopit = lopforx - 1;
  for (int i = 0; i < lopforx; i++) {
    randNumber = random(1, 7);
    activate = 1;
    delay(100);
    if (activate == 1) {
      switch (randNumber) {
        case 1:
          if(lopit==i) {tft.fillScreen(BLACK);}else{tft.fillScreen(YELLOW);}
          tft.fillRect(85, 120, 70, 70, RED);
          if(lopit==i) {tft.fillRect(0,280,240,40,CYAN);
          tft.setCursor (80, 290);
          tft.setTextSize (3);
          tft.setTextColor(WHITE);
          tft.println("Home");}
          activate = 0;
          break;
        case 2:
          if(lopit==i) {tft.fillScreen(BLACK);}else{tft.fillScreen(YELLOW);}
          tft.fillRect(140, 30, 70, 70, RED);
          tft.fillRect(30, 140, 70, 70, RED);
          if(lopit==i) {tft.fillRect(0,280,240,40,CYAN);
          tft.setCursor (80, 290);
          tft.setTextSize (3);
          tft.setTextColor(WHITE);
          tft.println("Home");}
          activate = 0;
          break;
        case 3:
          if(lopit==i) {tft.fillScreen(BLACK);}else{tft.fillScreen(YELLOW);}
          tft.fillRect(10, 170, 70, 70, RED);
          tft.fillRect(85, 90, 70, 70, RED);
          tft.fillRect(160, 10, 70, 70, RED);
          if(lopit==i) {tft.fillRect(0,280,240,40,CYAN);
          tft.setCursor (80, 290);
          tft.setTextSize (3);
          tft.setTextColor(WHITE);
          tft.println("Home");}
          activate = 0;
          break;
        case 4:
          if(lopit==i) {tft.fillScreen(BLACK);}else{tft.fillScreen(YELLOW);}
          tft.fillRect(30, 50, 70, 70, RED);
          tft.fillRect(140, 50, 70, 70, RED);
          tft.fillRect(30, 170, 70, 70, RED);
          tft.fillRect(140, 170, 70, 70, RED);
          if(lopit==i) {tft.fillRect(0,280,240,40,CYAN);
          tft.setCursor (80, 290);
          tft.setTextSize (3);
          tft.setTextColor(WHITE);
          tft.println("Home");}
          activate = 0;
          break;
        case 5:
          if(lopit==i) {tft.fillScreen(BLACK);}else{tft.fillScreen(YELLOW);}
          tft.fillRect(10, 10, 70, 70, RED);
          tft.fillRect(160, 10, 70, 70, RED);
          tft.fillRect(85, 105, 70, 70, RED);
          tft.fillRect(10, 210, 70, 70, RED);
          tft.fillRect(160, 210, 70, 70, RED); 
          if(lopit==i) {tft.fillRect(0,280,240,40,CYAN);
          tft.setCursor (80, 290);
          tft.setTextSize (3);
          tft.setTextColor(WHITE);
          tft.println("Home");}         
          activate = 0;
          break;
        case 6:
          if(lopit==i) {tft.fillScreen(BLACK);}else{tft.fillScreen(YELLOW);}
          tft.fillRect(30, 10, 70, 70, RED);
          tft.fillRect(130, 10, 70, 70, RED);
          tft.fillRect(30, 120, 70, 70, RED);
          tft.fillRect(130, 120, 70, 70, RED);
          tft.fillRect(30, 220, 70, 70, RED);
          tft.fillRect(130, 220, 70, 70, RED);
          if(lopit==i) {tft.fillRect(0,280,240,40,CYAN);
          tft.setCursor (80, 290);
          tft.setTextSize (3);
          tft.setTextColor(WHITE);
          tft.println("Home");}
          activate = 0;
          break;
      }
    }
  }
}
