#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// pin connection
#define up 3
#define down 4
#define left 2
#define right 5
#define a 6
#define speaker A0

// used for button values, high or low depending on whether pressed
int upVAL = 0;
int downVAL = 0;
int leftVAL = 0;
int rightVAL = 0;
int aVAL = 0;

int toDrunk = 32;
bool isDrunk = false;

// player intial position
int yPlayer = 1;
int xPlayer = 0;
int tick = 0;

// player points
int points = 0;
int drunkPoints = 0;

// non-player charachter (NPC) initial position
int yNPC = 0;
int xNPC = 18;
int yVK = 0;
int xVK = 18;

// NPC symbols
char npc = 'B';
char npcBad = 'R';
boolean isNPCbad = false;

boolean isGameOver = false;
boolean isPlayingMusic = false;

// create the player character
byte player[8] = {
  B01110,
  B01110,
  B00100,
  B11111,
  B10101,
  B00100,
  B11111,
  B10001,
};

byte vk[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte v1[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
};

byte v2[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
};
byte v3[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
};
byte v4[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte v5[8] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte v6[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
};
byte v7[8] = {
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte v8[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

char playerChar; // Used to store/call the player symbol
char v1Char;
char v2Char;
char v3Char;
char v4Char;
char v5Char;
char v6Char;
char v7Char;
char v8Char;

unsigned long moveTimePlayer = 175; // Number of milliseconds for Player to move
unsigned long nextMoveTimePlayer = 0;

unsigned long moveTimeNPC = 200; // Number of milliseconds for NPC to move
unsigned long nextMoveTimeNPC = 0;

void setup() {
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  
  lcd.createChar(10, player);
  playerChar = 10; 
  
  lcd.createChar(1, v1);
  v1Char = 1;
  lcd.createChar(20, v2);
  v2Char = 20;
  lcd.createChar(3, v3);
  v3Char = 3;
  
  lcd.createChar(4, v4);
  v4Char = 4;
  
  lcd.createChar(5, v5);
  v5Char = 5;
  lcd.createChar(6, v6);
  v6Char = 6;
  lcd.createChar(7, v7);
  v7Char = 7;
  lcd.createChar(8, v8);
  v8Char = 8;
// Set the player char as the player symbol
  
  Serial.begin(9600);

  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(a, INPUT_PULLUP);
  pinMode(speaker, OUTPUT);
  xVK = random(18); // 0 to 15
    yVK = random(0,3);  // 1 to 3
  reDraw();

}
 
void loop() {
  if (isGameOver) {
     lcd.clear();
     lcd.setCursor(5, 2);
     lcd.print("GAME OVER");
     playEndMusic();
  } else {
    detectButtonPresses();
  
    if (millis() > nextMoveTimeNPC) { // If enough time has passed for the NPC to move again
      tick++;
      moveNPC();
      nextMoveTimeNPC = millis() + moveTimeNPC; // Next time NPC can move is current timestamp plus the time it takes for an NPC to move (e.g. 1000 milliseconds)
      reDraw();
    
    } else {
      playMusic();
    }
  }
}

void playMusic() {

  // Notes for Mr Brightside riff
  #define F_sharp3 185
  #define C4 262
  #define C_sharp4 277
  #define F4 349
  #define C_sharp5 554
  #define D_sharp5 622

  const int songspeed = 2;

  // Mr Brightside
int notes[] = {
    C_sharp4, F4, C_sharp5, F4, D_sharp5, D_sharp5, F4, C_sharp5,
    C4, F4, C_sharp5, F4, D_sharp5, C_sharp5, F4, C_sharp5,
    F_sharp3, F4, C_sharp5, F4, D_sharp5, D_sharp5, F4, C_sharp5,
    F_sharp3, F4, C_sharp5, F4, D_sharp5, D_sharp5, F4, C_sharp5,
  };

  // Duration for each note
  int duration[] = {
    125, 125, 125, 125, 125, 125, 125, 125,
    125, 125, 125, 125, 125, 125, 125, 125,
    125, 125, 125, 125, 125, 125, 125, 125,
    125, 125, 125, 125, 125, 125, 125, 125,  
  };
/*
  for (int i=0; i<32; i++) {
    int wait = duration[i] * songspeed;
    tone(speaker, notes[i], wait);
    delay(wait);
  }
  */
  isPlayingMusic = false;

  int i = tick % 32;

  tone(speaker, notes[i], moveTimeNPC);
  delay(moveTimeNPC);
 
}


/**
  * Detects which buttons the player is pressing
  */
void detectButtonPresses() {
 
  upVAL = digitalRead(up);
      downVAL = digitalRead(down);
      leftVAL = digitalRead(left);
      rightVAL = digitalRead(right);
      aVAL = digitalRead(a);
  if(points > 3)
  {
    if(points % 2 == 0)
    {
        upVAL = digitalRead(down);
  downVAL = digitalRead(up);
  leftVAL = digitalRead(right);
  rightVAL = digitalRead(left);
  aVAL = digitalRead(a);
    }
   
  }

  if (upVAL == LOW) { // 'Up' button has been pressed
     movePlayer(0, -1);
  } else if (downVAL == LOW) { // 'Down' button has been pressed
     movePlayer(0, 1);
  } else if (leftVAL == LOW) { // 'Left' button has been pressed
     movePlayer(-1, 0);
  } else if (rightVAL == LOW) { // 'Right' button has been pressed
     movePlayer(1, 0);
  } else if (aVAL == LOW) { // 'A' button has been pressed
    // Do nothing
  }
  
}

/**
  * This function moves
  * the player position
  * at x and y axis.
  */
void movePlayer(int x, int y) {
  if (millis() > nextMoveTimePlayer) { // If enough time has passed for the Player to move again
    xPlayer += x;
    yPlayer += y;
    
    // Constrain the player by the bounds of the screen
    yPlayer = constrain(yPlayer, 0, 3);
    xPlayer = constrain(xPlayer, 0, 18);
    reDraw();
    nextMoveTimePlayer = millis() + moveTimePlayer; // Next time Player can move is current timestamp plus the time it takes for a Player to move (e.g. 175 milliseconds)
  } 
}

/**
  * Move the NPC
  * at x,y position 
  */
void moveNPC(){
  int xOrY = random(5); // 1 case out of 5 is 20% chance of moving up or down
  
  int amountToMove;
  
  switch(xOrY) {
    case 0: // Y 
      amountToMove = random(-1,2); // Random number from -1 to 1
      yNPC += amountToMove;
      yNPC = constrain(yNPC, 0, 3); // Constrain by the bounds of the screen
      break;
    default: // X
      int weightingRandom = random(1,101); // Depending on the NPC's position on the screen, it's next move is weighted. This is to use the full screen
      
      if (xNPC >= 12) { // Probably want to move left
        if (weightingRandom <= 20) { // 20% Chance of going right
          amountToMove = 1;
        } else { // 80% Chance of going left
          amountToMove = -1;
        }
      } else if (xNPC >= 8) { // Probably want to move left more
        if (weightingRandom <= 40) { // 40% Chance of going right
          amountToMove = 1;
        } else { // 60% Chance of going left
          amountToMove = -1;
        }
      } else if (xNPC <= 7) { // Probably want to move right
        if (weightingRandom <= 40) { // 40% Chance of going left
          amountToMove = -1;
        } else { // 60% Chance of going right
          amountToMove = 1;
        }
      }  else if (xNPC <= 3) { // Probably want to move right more
        if (weightingRandom <= 20) { // 20% Chance of going left
          amountToMove = -1;
        } else { // 80% Chance of going right
          amountToMove = 1;
        }
      }
      
      xNPC += amountToMove;
      xNPC = constrain(xNPC, 0, 18); // Constrain by the bounds of the screen
      break;
   }
   
}

void playEndMusic() {

  const int songspeed = 1.5;
 
  #define  a3f    208     // 208 Hz
  #define  b3f    233     // 233 Hz
  #define  b3     247     // 247 Hz
  #define  c4     261     // 261 Hz MIDDLE C
  #define  c4s    277     // 277 Hz
  #define  e4f    311     // 311 Hz    
  #define  f4     349     // 349 Hz
  #define  a4f    415     // 415 Hz  
  #define  b4f    466     // 466 Hz
  #define  b4     493     // 493 Hz
  #define  c5     523     // 523 Hz
  #define  c5s    554     // 554 Hz
  #define  e5f    622     // 622 Hz  
  #define  f5     698     // 698 Hz
  #define  f5s    740     // 740 Hz
  #define  a5f    831     // 831 Hz
  #define rest    -1

    int song1_chorus_melody[] = {
      b4f, b4f, a4f, a4f,
      f5, f5, e5f, b4f, b4f, a4f, a4f, e5f, e5f, c5s, c5, b4f,
      c5s, c5s, c5s, c5s,
      c5s, e5f, c5, b4f, a4f, a4f, a4f, e5f, c5s,
      b4f, b4f, a4f, a4f,
      f5, f5, e5f, b4f, b4f, a4f, a4f, a5f, c5, c5s, c5, b4f,
      c5s, c5s, c5s, c5s,
      c5s, e5f, c5, b4f, a4f, rest, a4f, e5f, c5s, rest
     };

    int song1_chorus_rhythmn[] = {
      125, 125, 125, 125,
      250, 250, 500, 125, 125, 125, 125, 250, 250, 250, 125, 250,
      1, 1, 1, 1,
      3, 3, 3, 1, 2, 2, 2, 4, 8,
      1, 1, 1, 1,
      3, 3, 6, 1, 1, 1, 1, 3, 3, 3, 1, 2,
      1, 1, 1, 1,
      3, 3, 3, 1, 2, 2, 2, 4, 8, 4
    };

    for (int i=0; i<59; i++) {
      int wait = song1_chorus_rhythmn[i] * songspeed;
      tone(speaker, song1_chorus_melody[i], wait);
      delay(wait);
    }
   
}

/**
  * reDraw functions, 
  * displays the points, player, character 
  * and NPC
  */
void reDraw() {
  
  // Clear the LCD screen
  lcd.clear();

  lcd.setCursor(19,3);



  
  drawVK();
  // Draw Points
  drawPoints();

//  drawDrunk();
  
  // Draw Player
  drawPlayer();
  
  // Work out whether Player and NPC are ontop of eachother
  if (playerNPCCollision()){ // Player and NPC at same position, give point and respawn NPC
    
    lcd.setCursor(0,3); // Set the LCD cursor position to bottom left to draw points given
  
   /* if(isNPCbad){ // We know NPC is bad, lose a point
      points -= 1;   
      lcd.print("-1");
   
      // Play random, long low tone between 50 and 100Hz
      int dur = random(50) + 50;
      int freq = random(50) + 50;
      
      tone(speaker, freq, dur); // Plays a tone
      delay(dur);  
    } else { // NPC is good, gain a point
      points += 1; 
      lcd.print("+1");
      
      // Play 25 random tones, one after another
      for(int i = 0; i < 25; i++) {    
        int dur = random(15) + 5;
        int freq = random(9080) + 20;
      
        tone(speaker, freq, dur); // Plays a tone
        delay(dur);
      }
      
    }
     
    // Generate a new random position for the NPC
    xNPC = random(19); // 0 to 15
    yNPC = random(1,3);  // 1 to 3
    */
    
    
    isGameOver = true;
  } 
  if((xVK == xPlayer) && (yVK == yPlayer))
  {
    xVK = random(19); // 0 to 15
    yVK = random(1,3);  // 1 to 3
    drawVK();
    points++;
    addPoints();
  }

  // Draw the NPC
  drawNPC();
}

/**
  * This function displays the 
  * score on the top right cner
  * of the LCD screen 
  */
void drawPoints() {
  String textPoints = String(points);
  
  // Padding
  if (points > 999) {       // Quad Digits
    lcd.setCursor(16,0);
  } else if (points > 99) { // Triple Digits
    lcd.setCursor(17,0);
  } else if (points > 9) {  // Double Digits
    lcd.setCursor(18,0);
  } else if (points < -99){ // Negative Triple Digits 
    lcd.setCursor(16,0);
  } else if (points < -9){  // Negative Double Digits  
    lcd.setCursor(17,0);
  } else if (points < 0){   // Negative Single Digit
    lcd.setCursor(18,0);
  } else {                  // One Digit
    lcd.setCursor(19,0);    
  }
  
  lcd.print(textPoints);
}

void addPoints()
{
  int toBeAdded = 1;
  if(points > 24)
  {
     lcd.clear();
     lcd.setCursor(5, 2);
     lcd.print("GAME OVER");
     playEndMusic();
  }
  else if(points > 16)
  {
    lcd.setCursor(19,1);
    if(points == 17)
    {
      lcd.write(v1);
    } else if (points == 18)
    {
      lcd.write(v2);
    } else if (points == 19)
    {
      lcd.write(v3);
    } else if (points == 20)
    {
      lcd.write(v4);
    } else if (points == 21)
    {
      lcd.write(v5);
    } else if (points == 22)
    {
      lcd.write(v6);
    } else if (points == 23)
    {
      lcd.write(v7);
    } else 
    {
      lcd.write(v8);
    }
    lcd.setCursor(19,2);
    lcd.write(v8);
    lcd.setCursor(19,3);
    lcd.write(v8);
    
    
  } else if (points > 8)
  {
    lcd.setCursor(19,1);
    if(points == 9)
    {
      lcd.write(v1);
    } else if (points == 10)
    {
      lcd.write(v2);
    } else if (points == 11)
    {
      lcd.write(v3);
    } else if (points == 12)
    {
      lcd.write(v4);
    } else if (points == 13)
    {
      lcd.write(v5);
    } else if (points == 14)
    {
      lcd.write(v6);
    } else if (points == 15)
    {
      lcd.write(v7);
    } else 
    {
      lcd.write(v8);
    }
    lcd.setCursor(19,3);
    lcd.write(v8);
  } else {
    lcd.setCursor(19,3);
    if(points == 1)
    {
      lcd.write(v1);
    } else if (points == 2)
    {
      lcd.write(v2);
    } else if (points == 3)
    {
      lcd.write(v3);
    } else if (points == 4)
    {
      lcd.write(v4);
    } else if (points == 5)
    {
      lcd.write(v5);
    } else if (points == 6)
    {
      lcd.write(v6);
    } else if (points == 7)
    {
      lcd.write(v7);
    } else 
    {
      lcd.write(v8);
    }
  }
}

/**
  * Draw the player 
  * at position x, y
  */
void drawPlayer() {
  // Set the LCD cursor position to where the Player should be drawn
  lcd.setCursor(xPlayer,yPlayer);
  
  // Print the character to the LCD.
  lcd.write(playerChar);  
}


/**
 * Draw the non-player Character
 * at position x, y
 */
void drawNPC() {
  // Set the LCD cursor position to where the NPC should be drawn
  lcd.setCursor(xNPC, yNPC);
  
    lcd.write(npc); 
  
}

void drawVK() {
  // Set the LCD cursor position to where the NPC should be drawn
  lcd.setCursor(xVK, yVK);
  
    lcd.write('V'); 
  
}


/**
 * Checks whether the
 * player charachter and NPC
 * are at the same possition and
 * returns true or false 
*/
boolean playerNPCCollision() {
  return (xNPC == xPlayer) && (yNPC == yPlayer);
}

boolean vkCollision() {
  return (xVK == xPlayer) && (yVK == yPlayer);
}
