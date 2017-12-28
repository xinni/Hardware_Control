//安装于灯板上，通过rf与PC端arduino通讯，接收控制灯板的命令
//指令格式：ON,2|2：打开4号灯板，2级亮度  OFF,2：关闭4号灯板（指令并非来自于PC）
//没有回复

#include "RF24.h"
#include "SPI.h"

//定义第二个灯板亮度控制的三位分别为 D2，D3，D4，开关位为D5
#define L1D0 2
#define L1D1 3
#define L1D2 4
#define L1Switch 5
//定义第四个灯板亮度控制的三位分别为 D6，D7，D8，开关位为A2
#define RELAY4 5
#define L2D0 6
#define L2D1 7
#define L2D2 8
#define L2Switch A2

// 定义第二个灯板的温度传感器接口为A4
#define L1TempSensor A4
// 定义第四个灯板的温度传感器接口为A5
#define L2TempSensor A5
// 定义关闭灯板电压为4V
float warningTemp = 2;
float errorTemp = 4;
unsigned long timerCount = 500000;

RF24 radio(9,10);
// 接收
const uint64_t pipe = 0xE6E6E6E6E603;
// 发送rf信息地址
const uint64_t MAIN = 0xE6E6E6E6E602;

char rcvMsg[32] = "";
String msg;

void setup() {
  
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
  
  if (timerCount > 1) {
    timerCount--;
  } else {
    TempDetect();
    timerCount = 500000;
  }
  //unsigned long nowtime=millis();
  
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
  brightness = brightness - 1;
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
      digitalWrite(L2Switch, LOW);
      if (brightness % 2) {
        digitalWrite(L2D0, HIGH);
      } else {
        digitalWrite(L2D0, LOW);
      }
      if ((brightness/2) % 2) {
        digitalWrite(L2D1, HIGH);
      } else {
        digitalWrite(L2D1, LOW);
      }
      if (((brightness/2)/2) % 2) {
        digitalWrite(L2D2, HIGH);
      } else {
        digitalWrite(L2D2, LOW);
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

void TempDetect() {
  /*两个灯板的温度测量，若高于50度则关闭灯板（温度显示为电压，目前未测试正确电压）*/
  // read the input on analog pin 4 and 5:
  int L1SensorValue = analogRead(A4);
  int L2SensorValue = analogRead(A5);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float L1voltage = L1SensorValue * (5.0 / 1023.0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float L2voltage = L2SensorValue * (5.0 / 1023.0);
  // 若测量值大于预设值则关闭灯板
  if (L1voltage > warningTemp) {
    char sendMsg[32];
    if (L1voltage > errorTemp) {
      TurnOff(1);
      String temp = "213 2|IR lamp temp error & off";
      temp.toCharArray(sendMsg, 32);
    } else {
      String temp = "212 2|IR lamp temperature error";
      temp.toCharArray(sendMsg, 32);
    }
    L1voltage = 0;
    radio.stopListening();
    radio.openWritingPipe(MAIN);
    radio.write(&sendMsg, sizeof(sendMsg));
    radio.openReadingPipe(1, pipe);
    radio.startListening();
  }
  if (L2voltage > warningTemp) {
    char sendMsg[32];
    if (L2voltage > errorTemp) {
      TurnOff(2);
      String temp = "213 4|IR lamp temp error & off";
      temp.toCharArray(sendMsg, 32);
    } else {
      String temp = "212 4|IR lamp temperature error";
      temp.toCharArray(sendMsg, 32);
    }
    L2voltage = 0;
    radio.stopListening();
    radio.openWritingPipe(MAIN);
    radio.write(&sendMsg, sizeof(sendMsg));
    radio.openReadingPipe(1, pipe);
    radio.startListening();
  }
}

