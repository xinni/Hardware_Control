#include "RF24.h"
#include "SPI.h"

RF24 radio(9,10);
const uint64_t LIGHT1 = 0xE6E6E6E6E601;
const uint64_t LIGHT2 = 0xE6E6E6E6E602;
const uint64_t LIGHT3 = 0xE6E6E6E6E603;
const uint64_t LIGHT4 = 0xE6E6E6E6E604;
String command = "";
String sendMessage = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  radio.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available()) {
    command += char(Serial.read());
    delay(10);
  }
  if (command.length() > 0) {
    if (GetOrder(command)) {
      command = "";
    } else {
      Serial.println("WRONG ORDER");
      command = "";
    }
  }
}

int GetOrder(String command) {
  if (command.startsWith("IR_ON")) {
    String number = command.substring(command.indexOf(' ')+1, command.indexOf(' ')+2);
    int num = number.toInt();
//  num is the number of the light
    String brightness = command.substring(command.indexOf(',')+1, command.length());
    int brightNum = brightness.toInt();
//  brightNum is the brightness of the light
    TurnOn(num, brightNum);
    command = "";
    return 1;
    
  } else if (command.startsWith("IR_OFF")) {
    String number = command.substring(command.indexOf(' ')+1, command.length());
    int num = number.toInt();
    TurnOff(num);
    command = "";
    return 1;
  } 
  
  else return 0;
}

void TurnOn (int num, int bright) {
  
  if (num == 1) {
    sendMessage = String("ON,"+bright);
    radio.openWritingPipe(LIGHT1);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else if (num == 2) {
    sendMessage = String("ON,"+bright);
    radio.openWritingPipe(LIGHT2);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else if (num == 3) {
    sendMessage = String("ON,"+bright);
    radio.openWritingPipe(LIGHT3);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else if (num == 4) {
    sendMessage = String("ON,"+bright);
    radio.openWritingPipe(LIGHT4);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else ;
  
  sendMessage = "";
}

void TurnOff (int num) {
  
  sendMessage = "OFF";
  if (num == 1) {
    radio.openWritingPipe(LIGHT1);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else if (num == 2) {
    radio.openWritingPipe(LIGHT2);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else if (num == 3) {
    radio.openWritingPipe(LIGHT3);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else if (num == 4) {
    radio.openWritingPipe(LIGHT4);
    radio.write(&sendMessage, sizeof(sendMessage));
    
  } else ;
  sendMessage = "";
}

