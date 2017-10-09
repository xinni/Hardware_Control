//安装于灯板上，通过rf与PC端arduino通讯，接收控制灯板的命令
//指令格式：ON,2|2：打开2号灯板，2级亮度  OFF,2：关闭2号灯板（指令并非来自于PC）
//没有回复

#include "RF24.h"
#include "SPI.h"

//定义第一个灯板亮度控制的三位分别为 D2，D3，D4，开关位为D5
#define L1D0 2
#define L1D1 3
#define L1D2 4
#define L1Switch 5
//定义第二个灯板亮度控制的三位分别为 D6，D7，D8，开关位为A2
#define RELAY4 5
#define L2D0 6
#define L2D1 7
#define L2D2 8
#define L2Switch A2

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
  pinMode(L1D0, OUTPUT);
  pinMode(L1D1, OUTPUT);
  pinMode(L1D2, OUTPUT);
  pinMode(L1Switch, OUTPUT);
  digitalWrite(L1Switch, HIGH);
  pinMode(L2D0, OUTPUT);
  pinMode(L2D1, OUTPUT);
  pinMode(L2D2, OUTPUT);
  pinMode(L2Switch, OUTPUT);
  digitalWrite(L2Switch, HIGH);
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
    if (brightness >= 0 & brightness <= 7){
      digitalWrite(L1Switch, LOW);
      if (brightness % 2) {
        digitalWrite(L1D0, HIGH);
      } else {
        digitalWrite(L1D0, LOW);
      }
      if ((brightness/2) % 2) {
        digitalWrite(L1D1, HIGH);
      } else {
        digitalWrite(L1D1, LOW);
      }
      if (((brightness/2)/2) % 2) {
        digitalWrite(L1D2, HIGH);
      } else {
        digitalWrite(L1D2, LOW);
      }
    } else { }
    msg = "";
  } else if (num == 2) {
    if (brightness >= 0 & brightness <= 7){
      digitalWrite(L1Switch, LOW);
      if (brightness % 2) {
        digitalWrite(L1D0, HIGH);
      } else {
        digitalWrite(L1D0, LOW);
      }
      if ((brightness/2) % 2) {
        digitalWrite(L1D1, HIGH);
      } else {
        digitalWrite(L1D1, LOW);
      }
      if (((brightness/2)/2) % 2) {
        digitalWrite(L1D2, HIGH);
      } else {
        digitalWrite(L1D2, LOW);
      }
    } else { }
    msg = "";
  } else msg = "";
}

void TurnOff(int num) {
  if (num == 1) {
    //关闭所有继电器
    digitalWrite(L1D0, LOW);
    digitalWrite(L1D1, LOW);
    digitalWrite(L1D2, LOW);
    digitalWrite(L1Switch, HIGH);
    msg = "";
  } else if (num == 2) {
    //关闭所有继电器
    digitalWrite(L2D0, LOW);
    digitalWrite(L2D1, LOW);
    digitalWrite(L2D2, LOW);
    digitalWrite(L2Switch, HIGH);
    msg = "";
  } else msg = "";

}

