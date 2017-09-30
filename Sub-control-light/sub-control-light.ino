//安装于灯板上，通过rf与PC端arduino通讯，接收控制灯板的命令
//指令格式：ON,2|2：打开2号灯板，2级亮度  OFF,2：关闭2号灯板（指令并非来自于PC）
//没有回复

#include "RF24.h"
#include "SPI.h"

//定义第一个灯板的三个继电器接脚分别为 2，3，4
#define RELAY1 2
#define RELAY2 3
#define RELAY3 4
//定义第二个灯板的三个继电器接脚分别为 5，6，7
#define RELAY4 5
#define RELAY5 6
#define RELAY6 7

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
      String number = msg.substring(msg.indexOf(',')+1, msg.indexOf('|'));
      int lightNum = number.toInt();
      String brightness = msg.substring(msg.indexOf('|')+1, msg.length());
      int brightNum = brightness.toInt();
      TurnOn(lightNum, brightNum);
    } else if (msg.startsWith("OFF")) {
      String number = msg.substring(msg.indexOf(',')+1, msg.length());
      int lightNum = number.toInt();
      TurnOff(lightNum);
    } else msg = "";
  }
}

void TurnOn(int num, int brightness) {
  if (num == 1) {
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
  } else if (num == 2) {
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
  } else msg = "";
}

void TurnOff(int num) {
  if (num == 1) {
    //关闭所有继电器
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    msg = "";
  } else if (num == 2) {
    //关闭所有继电器
    digitalWrite(RELAY4, LOW);
    digitalWrite(RELAY5, LOW);
    digitalWrite(RELAY6, LOW);
    msg = "";
  } else msg = "";

}

