//#include <SoftwareSerial.h> // for Bluetooth Serial Comm.
//#include <Servo.h>

//SoftwareSerial mySerial(2,3); // Rx | Tx For Bluetooth

//Servo UpDown; // Gripper
//Servo OpenClose; // Gripper
//Servo took; 

const byte numChars = 50;
char receivedChars[numChars];
char tempChars[numChars];

int power; // for Basement motors
int D1C1;
int D2C2;
int A1B1;
int A2B2;
/*
String gp_servo_var; // for data parsing OF GRIPPER
//unsigned long lastUpdate = 0; // For servo motor signals, more efficient
//unsigned long updatePeriod = 50; // For servo motor signals, more efficient
int gp_UpDown_var = 90;
int gp_OpenClose_var = 90;

//----------------------------------------
//----------------------------------------

int UpDown_pin = 9; // for Servo motors of Grippers
int OpenClose_pin = 10;*/

//-----------------------------------------
//-----------------------------------------
//FOR elevator ( ele )
int ele_enable_pin = 5; 
int ele_dir_pin = 4;

int ele_enable; // For storing values
int ele_dir;

String ele_var; // For holding data received

//-----------------------------------------
//-----------------------------------------
//-------------------------------------------
/*
String took_var; //HOOK HOOK HOOK

const int took_pin = 11; // for servo with Alan Keys

int took_pos = 160; */

//-------------------------------------------
//-------------------------------------------
//-------------------------------------------

boolean newData = false;

// -------------------------------------------
// Digital OUTPUT
int power_pin = 6;
int D1C1_pin  = 13; // motors for basement
int D2C2_pin  = 12;
int A1B1_pin  = 8;
int A2B2_pin  = 7;
// -------------------------------------------

void setup() {
  //mySerial.begin(38400);
  Serial3.begin(38400); // 14 & 15
  Serial.begin(38400);

  /*UpDown.attach(UpDown_pin); // For Servo motors
  OpenClose.attach(OpenClose_pin);
  
  UpDown.write(gp_UpDown_var);
  OpenClose.write(gp_OpenClose_var);// FOR SERVO MOTORS GRIPPER*/
  
  pinMode(power_pin, OUTPUT);
  pinMode(D1C1_pin, OUTPUT);
  pinMode(D2C2_pin, OUTPUT);
  pinMode(A1B1_pin, OUTPUT);
  pinMode(A2B2_pin, OUTPUT);

  pinMode(ele_enable_pin, OUTPUT);
  pinMode(ele_dir_pin, OUTPUT);
/*
  // HOOK PIN PIN PIN HOOK
  took.attach(took_pin);
  took.write(took_pos); //INITIAL POSITION*/
}

void loop() {
  recvWithStartEndMarkers();
  if ( newData == true ) {
    strcpy( tempChars, receivedChars);
    Serial.println(receivedChars);
    parseData();
    outputParsedData();
    newData = false;
  }

}


// -------------------------------------------

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while ( Serial3.available() > 0 && newData == false ) {
    rc = Serial3.read();

    if ( recvInProgress == true ) {
      if ( rc != endMarker ) {
        receivedChars[ndx] = rc;
        ndx++;
        if ( ndx >= numChars ) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0';
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }
    else if ( rc == startMarker ) {
      recvInProgress = true;
    }
  }
}

void parseData() {
  
  char * strtokIndx;

  char dummy = tempChars[0];
  
  if ( (dummy == '1') || (dummy == '0' ) ) {  
    strtokIndx = strtok(tempChars, ",");
    power = atoi(strtokIndx);
    
    strtokIndx = strtok(NULL , ",");
    D1C1 = atoi(strtokIndx);

    strtokIndx = strtok(NULL , ",");
    D2C2 = atoi(strtokIndx);

    strtokIndx = strtok(NULL , ",");
    A1B1 = atoi(strtokIndx);

    strtokIndx = strtok(NULL , ">");
    A2B2 = atoi(strtokIndx);
  }
  /*
  if ( dummy == 'g') { // GRIPPER
    strtokIndx = strtok(tempChars, ">");
    gp_servo_var = (String) strtokIndx;
  }*/

  if ( dummy == 'e') { // ELEVATOR
    strtokIndx = strtok(tempChars, ">");
    ele_var = (String) strtokIndx;
  }
/*
  if ( dummy == 't' ) { // HOOK!!!!!!!!!!!!!!!!!!!!!!
    strtokIndx = strtok(tempChars, ">");
    took_var = (String) strtokIndx; 
  }*/
}

//--------------------------------------
// EFFECTOR
void outputParsedData() {

  if ( power > 0 ) {
    analogWrite(power_pin, 180);//ANAlog 90
  }
  else {
    digitalWrite(power_pin, power);
  }
  
  digitalWrite(D1C1_pin, D1C1);
  digitalWrite(D2C2_pin, D2C2);//-For basement motor
  digitalWrite(A1B1_pin, A1B1);
  digitalWrite(A2B2_pin, A2B2);

  /*
  //unsigned int now = millis();
  //if ( (now - lastUpdate) >= updatePeriod ) {
    if (gp_servo_var == "gp_up" ) {// SERVO OF GRIPPER
      gp_UpDown_var = gp_UpDown_var + 1;
      UpDown.write(gp_UpDown_var);  
    }
    else if (gp_servo_var == "gp_down") {
      gp_UpDown_var = gp_UpDown_var - 1;
      UpDown.write(gp_UpDown_var);
    }
    else if (gp_servo_var == "gp_open") {
      gp_OpenClose_var = gp_OpenClose_var + 1;
      OpenClose.write(gp_OpenClose_var);
    }
    else if (gp_servo_var == "gp_close") {
      gp_OpenClose_var = gp_OpenClose_var - 1;
      OpenClose.write(gp_OpenClose_var);
    }
    else if (gp_servo_var == "gp_remain") {
    }
    //lastUpdate = millis();
  //}

    // HOOK SERVO HOOK SERVO HOOK SERVO HOOK SERVO
  if ( took_var == "took_up" ) {
    took_pos = took_pos + 1;
    took.write(took_pos);
  }
  if ( took_var == "took_down") {
    took_pos = took_pos - 1;
    took.write(took_pos);
  }
  */

  // ELEVATOR 
  if (ele_var == "ele_lower") {
    analogWrite(ele_enable_pin, 150); // ANAlog 127
    digitalWrite(ele_dir_pin, 0);
  }
  else if (ele_var == "ele_raise") {
    analogWrite(ele_enable_pin, 150); // ananlog127
    digitalWrite(ele_dir_pin, 1);
  }
  else if (ele_var == "ele_stop") {
    analogWrite(ele_enable_pin, 0); // analog 0
  }


}

