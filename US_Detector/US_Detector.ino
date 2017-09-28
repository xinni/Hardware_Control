//安装于超声测距板上，通过rf与PC端arduino通讯，接收控制超声波模块的命令
//指令格式： US_ON 打开超声测距，并回传信息 US_OFF 关闭超声测距
//收到打开指令后，开始测距，并回传距离信息，收到关闭指令后关闭超声测距
//回复格式：102 50：表示超音波距離偵測完成。距離為50公分(cm)

#include "RF24.h"
#include "SPI.h"

//超声波模块ID，需要与超声波控制端ID相对应
const uint64_t US = 0x56E6E6E6E601;

RF24 radio(9,10);
const int TrigPin = 2;
const int EchoPin = 3;
boolean state = false;
float cm;
String rcvMsg = "";

void setup() {
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  radio.begin();
}

void loop() {
  rcvMsg = rcvOrder();
  if (rcvMsg.length() > 0) {
    if (rcvMsg == "US_ON") {
      state = true;
      radio.stopListening();
      radio.openWritingPipe(US);
      rcvMsg = "";
      
    } else if (rcvMsg == "US_OFF") {
      state = false;
      radio.openReadingPipe(1, US);
      radio.startListening();
      rcvMsg = "";
    }
  }
  if (state) {
    DetectDist();
  }
}

void DetectDist () {
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2); 
  digitalWrite(TrigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(TrigPin, LOW); 

  cm = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
  cm = (int(cm * 100.0)) / 100.0; //保留两位小数 
  String dist = String("102 "+(String)cm);
  radio.write(&dist, sizeof(dist));
  delay(1); 
}

String rcvOrder() {
  String receiveMsg = "";
  radio.openReadingPipe(1, US);
  radio.startListening();
  while(radio.available()) {
    radio.read(&receiveMsg, sizeof(receiveMsg));
  }
  return receiveMsg;
}

