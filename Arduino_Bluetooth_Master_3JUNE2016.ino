#include <SoftwareSerial.h>

//--------------------------------
//--------------------------------
#include <math.h>
#include <stdio.h>
#include <avr/io.h>
//--------------------------------
//--------------------------------
#define LED_PIN 13
#define DELAY(wait) digitalWrite(LED_PIN,LOW); delay(wait); digitalWrite(LED_PIN,HIGH);

/* These are AVR PORTB pins, +8 to convert to Arduino pins */
#define PS2clk 5
#define PS2cmd 3
#define PS2att 2
#define PS2dat 4
#define PS2PORT PORTB
#define PS2IN PINB
#define CTRL_CLK 20
#define CTRL_BYTE_DELAY 20

//These are our button constants
#define PSB_SELECT 0x01
#define PSB_L3 0x02
#define PSB_R3 0x04
#define PSB_START 0x08
#define PSB_PAD_UP 0x10
#define PSB_PAD_RIGHT 0x20
#define PSB_PAD_DOWN 0x40
#define PSB_PAD_LEFT 0x80

#define PSB_L2 0x100
#define PSB_R2 0x200
#define PSB_L1 0x400
#define PSB_R1 0x800
#define PSB_GREEN 0x1000
#define PSB_RED 0x2000
#define PSB_BLUE 0x4000
#define PSB_PINK 0x8000

#define SET(x,y) (x|=(1<<y))
#define CLR(x,y) (x&=(~(1<<y)))
#define CHK(x,y) (x & (1<<y))
#define TOG(x,y) (x^=(1<<y)) 

boolean PSButton();
unsigned char PS2data[9];
void read_gamepad();
void config_gampad();
unsigned char get_gamepad_mode();
unsigned char i;
//-----------------------------------
//--------------------------------

const byte rxPin = 2;
const byte txPin = 3;
SoftwareSerial mySerial(rxPin,txPin); // RX|TX
char c;

const int SFpin = 9;
const int SBpin = 8;
const int SLpin = 7;
const int SRpin = 6;

char SF;
char SB;
char SL;
char SR;

//------------------------------------
// SERVO MOTOR GRIPPER

//------------------------------------

// byte packet[5];
char power;
char D1C1;
char D2C2;
char A1B1;
char A2B2;

// FOR Enhancemenet of Controller
int counter; // to remove jittering
int inverse; // to inverse control direction

void setup() {
  
  pinMode(SFpin,INPUT);
  pinMode(SBpin,INPUT);
  pinMode(SLpin,INPUT);
  pinMode(SRpin,INPUT);

  //Serial.begin(38400);
  mySerial.begin(38400);

  //---------------------------------
  //--------------------------------
  randomSeed(analogRead(0));
   Serial.begin(38400);    //originally 57600
   pinMode(LED_PIN,OUTPUT);
   digitalWrite(LED_PIN,HIGH);

   pinMode(PS2clk+8,OUTPUT);
   pinMode(PS2att+8,OUTPUT);
   pinMode(PS2cmd+8,OUTPUT);
   pinMode(PS2dat+8,INPUT);
   digitalWrite(PS2dat+8,HIGH);

   counter = 0;
   inverse = 0;

   config_gampad();
   //--------------------------------
  //---------------------------------
}

void loop() {
  //debug..................................
  /*if ( Serial.available() > 0 ) {
    c = Serial.read();
    mySerial.write(c);
    Serial.write(c);
  }
  if ( mySerial.available() > 0 ) {
    c = mySerial.read();
    Serial.write(c);
  } */
  //debug..................................

  //--------------------------------
  //----------------------------------------
  
    read_gamepad();  
    if(PSButton(PSB_SELECT)) {
      mySerial.write("<ele_lower>");
      Serial.print("SELECT\n");
      counter = 0;
    }
    if(PSButton(PSB_L3)) {
      Serial.print("L3\n");
      counter = 0;
    }
    if(PSButton(PSB_R3)) {
      Serial.print("R3\n");
      counter = 0;
    }
    if(PSButton(PSB_START)) {
      mySerial.write("<ele_raise>");
            Serial.print("START\n");
      counter = 0;
    }
      //--
  if ( inverse == 0 ) {    
    if(PSButton(PSB_PAD_UP)) {
      mySerial.write("<1,0,0,1,1>");
            Serial.print("PAD_UP\n");
      counter = 0;
    }
    if(PSButton(PSB_PAD_RIGHT)) {
      mySerial.write("<1,1,1,1,1>");
      Serial.print("PAD_RIGHT\n");
      counter = 0;
    }
    if(PSButton(PSB_PAD_LEFT)) {
      mySerial.write("<1,0,0,0,0>");
      Serial.print("PAS_LEFT\n");
      counter = 0;
    }
    if(PSButton(PSB_PAD_DOWN)) {
      mySerial.write("<1,1,1,0,0>");
      Serial.print("PAD_DOWN\n");
      counter = 0;
    }
      delay(10);
  }
  if ( inverse == 1 ) {
    if(PSButton(PSB_PAD_UP)) {
      mySerial.write("<1,1,1,0,0>");
      counter = 0;
    }
    if(PSButton(PSB_PAD_RIGHT)) {
      mySerial.write("<1,1,1,1,1>");
      counter = 0;
    }
    if(PSButton(PSB_PAD_LEFT)) {
      mySerial.write("<1,0,0,0,0>");
      counter = 0;
    }
    if(PSButton(PSB_PAD_DOWN)) {
      mySerial.write("<1,0,0,1,1>");
      counter = 0;
    }
      delay(10);
  }
      //--
    if(PSButton(PSB_L1)) {
      inverse = inverse + 1;
      inverse = inverse % 2;
      counter = 0;
      Serial.print("L1\n");
    }
    if(PSButton(PSB_R1)) {
      mySerial.write("<took_up>");
      Serial.print("R1\n");
      counter = 0;
    }
    if(PSButton(PSB_L2)) {
      Serial.print("L2\n");
      counter = 0;
    }
    if(PSButton(PSB_R2)) {
      mySerial.write("<took_down>");
      Serial.print("R2\n");
      counter = 0;
    }
    if(PSButton(PSB_GREEN)) {
      mySerial.write("<gp_up>");
      Serial.print("GREEN\n");
      counter = 0;
    }
    if(PSButton(PSB_RED)) {
      mySerial.write("<gp_open>");
      Serial.print("RED\n");
      counter = 0;
    }
    if(PSButton(PSB_PINK)) {
      mySerial.write("<gp_close>");
      Serial.print("SQUARE\n");
      counter = 0;
    }
    if(PSButton(PSB_BLUE)) {
      mySerial.write("<gp_down>");
      Serial.print("blue\n");
      counter = 0;
    }
    else {
      if ( counter < 5 ) {
        mySerial.write("<0,0,0,0,0>");
        mySerial.write("<gp_remain>");
        mySerial.write("<ele_stop>");
        counter = counter + 1;
      }
    }
      //--------------------------------
    //-----------------------------------------
/*
  SF = digitalRead(SFpin);
  SB = digitalRead(SBpin);
  SL = digitalRead(SLpin);
  SR = digitalRead(SRpin);

  
  if ( (((SF==1)&&(SB==0))&&(SL==0))&&(SR==0) ) { // GO FORWARD
    mySerial.write("<1,1,0,1,0>");
  }
  else if ( (((SF==0)&&(SB==1))&&(SL==0))&&(SR==0) ) { // GO BACKWARD
    mySerial.write("<1,0,1,0,1>");
  }
  else if ( (((SF==0)&&(SB==0))&&(SL==1))&&(SR==0) ) { // TURN LEFT
    mySerial.write("<1,0,1,1,0>");
  }
  else if ( (((SF==0)&&(SB==0))&&(SL==0))&&(SR==1) ) { // TURN RIGHT
    mySerial.write("<1,1,0,0,1>");
  }
  else if ( (((SF==0)&&(SB==0))&&(SL==0))&&(SR==0) ) {
    mySerial.write("<0,0,0,0,0>");
  }
  else {
  }
*/
    delay(2);
}


//-------------------------------------------------------
//-------------------------------------------------------
boolean PSButton(unsigned int button) {
int byte = 3;
if (button >= 0x100) {
 byte = 4;
 button = button >> 8;
}
 if (~PS2data[byte] & button) 
 return true;
 else
 return false;
}
unsigned char _gamepad_shiftinout (char byte) {
  unsigned char tmp = 0;
  for(i=0;i<8;i++) {
     if(CHK(byte,i)) SET(PS2PORT,PS2cmd);
       else  CLR(PS2PORT,PS2cmd);
     CLR(PS2PORT,PS2clk); 
     delayMicroseconds(CTRL_CLK); 
       if(CHK(PS2IN,PS2dat)) SET(tmp,i); 
     SET(PS2PORT,PS2clk); 
  }
  SET(PS2PORT,PS2cmd); 
  delayMicroseconds(CTRL_BYTE_DELAY);
  return tmp;
}
void _gamepad_shiftout (char byte) {
  for(i=0;i<8;i++) {
     if(CHK(byte,i)) SET(PS2PORT,PS2cmd);
       else  CLR(PS2PORT,PS2cmd);
     CLR(PS2PORT,PS2clk); 
     delayMicroseconds(CTRL_CLK); 
     SET(PS2PORT,PS2clk); 
     //delayMicroseconds(CTRL_CLK); 
  }
  SET(PS2PORT,PS2cmd); 
  delayMicroseconds(CTRL_BYTE_DELAY);
}
unsigned char _gamepad_shiftin() {
  unsigned char tmp = 0;
  for(i=0;i<8;i++) {
     CLR(PS2PORT,PS2cmd);
     CLR(PS2PORT,PS2clk); 
     delayMicroseconds(CTRL_CLK);
       if(CHK(PS2IN,PS2dat)) SET(tmp,i); 
     SET(PS2PORT,PS2clk); 
     delayMicroseconds(CTRL_CLK);
  }
  SET(PS2PORT,PS2cmd); 
  delayMicroseconds(CTRL_BYTE_DELAY);
  return tmp;
}
void read_gamepad() {
   SET(PS2PORT,PS2cmd); 
   SET(PS2PORT,PS2clk); 
   CLR(PS2PORT,PS2att); // low enable joystick
   delayMicroseconds(CTRL_BYTE_DELAY);
   char dword[9] = {0x01,0x42,0,0,0,0,0,0,0};
   for (int i = 0; i<9; i++) {
       PS2data[i] = _gamepad_shiftinout(dword[i]);
   }
   SET(PS2PORT,PS2att); // HI disable joystick
}
unsigned char get_gamepad_mode() {
   SET(PS2PORT,PS2cmd); 
   SET(PS2PORT,PS2clk); 
   CLR(PS2PORT,PS2att); // low enable joystick
   _gamepad_shiftout(0x01);
   unsigned char x = _gamepad_shiftin();
   SET(PS2PORT,PS2att); // HI disable joystick
   return x;
}
void config_gampad() {
  SET(PS2PORT,PS2cmd); 
  SET(PS2PORT,PS2clk); 
  CLR(PS2PORT,PS2att); // low enable joystick
   _gamepad_shiftout(0x01);
   _gamepad_shiftout(0x43);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x01);
   _gamepad_shiftout(0x00);
   // Lock to Analog Mode on Stick
   _gamepad_shiftout(0x01);
   _gamepad_shiftout(0x44);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x01);
   _gamepad_shiftout(0x03);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x00);
   // Vibration
   /*
   _gamepad_shiftout(0x01);
   _gamepad_shiftout(0x4D);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x01);
   */
   _gamepad_shiftout(0x01);
   _gamepad_shiftout(0x4F);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0xFF);
   _gamepad_shiftout(0xFF);
   _gamepad_shiftout(0x03);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x01);
   _gamepad_shiftout(0x43);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x00);
   _gamepad_shiftout(0x5A);
   _gamepad_shiftout(0x5A);
   _gamepad_shiftout(0x5A);
   _gamepad_shiftout(0x5A);
   _gamepad_shiftout(0x5A);
  SET(PS2PORT,PS2att);
}
//-------------------------------------------------------
//-------------------------------------------------------
