//通过串口与PC端通信(Baud rate:115200)，持续送回超声波所侦测到的距离
//指令格式：US_OFF 关闭超音波 US_ON 打开超音波
//收到关闭指令(US_OFF)后，中断回复，等待下次命令(US_ON)
//回复格式：102 50：表示超音波距離偵測完成。距離為50公分(cm)

#include "RF24.h"
#include "SPI.h"

//所对应的超声模块中ID需要与下面ID相对应
const uint64_t US = 0x56E6E6E6E601;

RF24 radio(9,10);
String command = "";
String sendMsg = "";
String rcvMsg = "";

void setup() {
  Serial.begin(115200);
  radio.begin();
}

void loop() {
  while(Serial.available()) {
    command += char(Serial.read());
    delay(10);
  }
  if (command.length() > 0) {
    if (command == "US_ON") {
      radio.stopListening();
      sendMsg = "US_ON";
      radio.openWritingPipe(US);
      radio.write(&sendMsg, sizeof(sendMsg));
      radio.openReadingPipe(1, US);
      radio.startListening();
      command = "";
      
    } else if (command == "US_OFF") {
      radio.stopListening();
      sendMsg = "US_OFF";
      radio.openWritingPipe(US);
      radio.write(&sendMsg, sizeof(sendMsg));
      command = "";
    } else command = "";
  }
  if (radio.available()) {
    radio.read(&rcvMsg, sizeof(rcvMsg));
    Serial.println(rcvMsg);
  }
}



