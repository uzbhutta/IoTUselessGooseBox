#include "application.h"
#include "ArduinoJson.h"
#include "actions.h"
#include "SparkIntervalTimer.h"
#include <string.h>

# define MODULES  4
//-------------------------------
// VARIABLES
//-------------------------------
enum TYPE { PRESET = 0, CUSTOM, INVALID };
TYPE typeFlag;
IntervalTimer threadTimer;

//pin mapping
int pin_switch = 7;
int pin_servoLid1 = 9;
int pin_servoArm = 10;
int pin_lidLight = 2;
int pin_redLight = 4;

//arrays
String customArray[6];
int customArrayNumItems = 0;
int presetArrayNumItems = 0;

Servo servoLid1, servoLid2, servoArm;
int pos; // variable to store the servo position


//-------------------------------
// MULTITHREADING INTERVALTIMER
//-------------------------------
void threadTimerCB()
{
  //ToDo: call on LED/light functions from new thread
}

//-------------------------------
// SETUP
//-------------------------------
void setup() {
  Particle.function("jsonParser", jsonParser);
  Serial.begin(9600);

  //pin init
  pinMode(pin_switch, INPUT);
  pinMode(pin_lidLight, OUTPUT);
  pinMode(pin_redLight, OUTPUT);

  //Attach lid servo
  servoLid1.attach(pin_servoLid1);
  //Attach arm servo
  servoArm.attach(pin_servoArm);
}

//-------------------------------
// MAIN LOOP
//-------------------------------
void loop() {
  if (digitalRead(pin_switch) == HIGH)
  {
    Serial.println ("switch is on");
    if (typeFlag == CUSTOM ){
      runCustomSequence();
    }
    else //preset or invalid
    {
      runPresetSequence();
    }
  }
}

//-------------------------------
// HELPER FUNCTIONS
//-------------------------------
void runCustomSequence() {
  //pick an array element from customArray[] at random
  //for each character in that element, run the approproate Action static class function, keep multithreading in mind

  //generate random index number between 0 and size of customArray
  int rando = random(0, customArrayNumItems);
  int index = 0;
  const char * charArray = customArray[rando].c_str(); //convert string in customArray to an array of chars
  char letter;

  Serial.print("Running Custom Sequence: ");
  Serial.println(rando);

  //parse through characters of string in random index
  for(int i = 0; i < MODULES; i++){  //REV.IEW PARSING**//
    //send each character of string to appropriate action function depending on the where the char is in the string
    letter = charArray[i];
    switch (i){
      case 0:
        Action::actuateLid(letter);
        break;
      case 1:
        Action::actuateLidLED(letter);
        break;
      case 2:
        Action::actuateRedLED(letter);
        break;
      case 3:
        Action::actuateArm(letter);
        break;
    }
  }
}

void runPresetSequence() {
  //randomize a number between 0 and number of preset functions (5)
  //based on randomized number, run appropriate Action static class function (pertaining to presets)

  int rando = random(0, presetArrayNumItems); //random number generator between 0 and size of customArray
  Serial.print("Running Preset Sequence: ");
  Serial.println(rando);

  switch (rando)
  {
    case 0:
      //ToDo: run preset function 0
      break;
    case 1:
      //ToDo: run preset function 1
      break;
    case 2:
      //ToDo: run preset function 2
      break;
    case 3:
      //ToDo: run preset function 3
      break;
    case 4:
      //ToDo: run preset function 4
      break;
  }
}

int jsonParser(String jsonInput)
{
  //JSON format: {"type":1, "data":["AAAA","ABCA","DCAC","ABCD","DCAC","ACDC"]}
  //If type = 0, set typeFlag to 0, return 0.
  //Else, set typeFlag to 1, store data JsonArray in customArray[] dynamically as below, return 1. Else, return -1.

  StaticJsonBuffer<64> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(const_cast<char*>(jsonInput.c_str()));

  Serial.println(root["type"].as<int>());

  if (!root.success())
  {
    Serial.println("ERROR: Unable to parse input.");
    return -1;
  }

  if (root["type"] == 0)
  {
    typeFlag = PRESET;
    return 0;
  }
  else if (root["type"] == 1)
  {
    typeFlag = CUSTOM;
    JsonArray& dataArray = root["data"].asArray();
    customArrayNumItems = dataArray.size();

    for (int i = 0; i < customArrayNumItems; i++)
    {
      customArray[i] = dataArray[i];

      //for testing purposes
      Serial.println("Successfully stored rows from cloud input:");
      Serial.println(customArray[i]);
    }

    return 1;
  }
  else
  {
    typeFlag = INVALID;
    Serial.println("ERROR: The 'type' value is invalid.");
    return -1;
  }
}
