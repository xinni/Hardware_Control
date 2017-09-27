#include "RF24.h"
#include "SPI.h"

//定义三个继电器的接脚分别为 6，7，8
#define RELAY1 6
#define RELAY2 7
#define RELAY3 8

RF24 radio(9,10);
const uint64_t pipe = 0xE6E6E6E6E601;
String command = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (radio.available()) {
    radio.read(&command, sizeof(command));
    if (command.startsWith("ON")) {
      String brightness = command.substring(command.indexOf(',')+1, command.length());
      int brightNum = brightness.toInt();
      TurnOn(brightNum);
    } else if (command == "OFF") {
      TurnOff();
    } else command = "";
  }
}

void TurnOn(int brightness) {
  if (brightness == 1){
    //1级亮度
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
  } else if (brightness == 2) {
    //2级亮度
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, LOW);
  } else if (brightness == 3) {
    //3级亮度
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, HIGH);
    digitalWrite(RELAY3, HIGH);
  } else ;
  command = "";
}

void TurnOff() {
  //关闭所有继电器
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  command = "";
}

