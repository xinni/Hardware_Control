//安装于灯板上，通过rf与PC端arduino通讯，接收控制灯板的命令
//指令格式：ON,2：打开灯板，2级亮度  OFF：关闭灯板（指令并非来自于PC）
//没有回复

#include "RF24.h"
#include "SPI.h"

//定义三个继电器的接脚分别为 2，3，4
#define RELAY1 2
#define RELAY2 3
#define RELAY3 4

RF24 radio(9,10);
const uint64_t pipe = 0xE6E6E6E6E601;
char rcvMsg[32] = "";
String msg;

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
    radio.read(&rcvMsg, sizeof(rcvMsg));
    msg = String(rcvMsg);
    if (msg.startsWith("ON")) {
      String brightness = msg.substring(msg.indexOf(',')+1, msg.length());
      int brightNum = brightness.toInt();
      TurnOn(brightNum);
    } else if (msg == "OFF") {
      TurnOff();
    } else msg = "";
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
  msg = "";
}

void TurnOff() {
  //关闭所有继电器
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  msg = "";
}

