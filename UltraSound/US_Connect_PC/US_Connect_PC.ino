//通过串口与PC通信(Baud rate:115200),接收PC端开关红外线灯板的命令
//指令格式：IR_ON 2,3：表示打开2号灯板，3级亮度  IR_OFF 2：关闭2号灯板
//没有回复

#include "RF24.h"
#include "SPI.h"

const uint64_t USC = 0xE6E6E6E6E666;
//超声板程序中的ID需要与下面的ID相对应
const uint64_t US1 = 0xE6E6E6E6E677;
const uint64_t US2 = 0xE6E6E6E6E678;
const uint64_t US3 = 0xE6E6E6E6E679;

int usStatus[3] = {0,0,0};

RF24 radio(9,10);
String command = "";
String serialCommand = "";
String sendMessage = "";
char rcvMsg[32] = "";

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(1, USC);
  radio.startListening();
}

void loop() {
  
  while (Serial.available()) {
    serialCommand += char(Serial.read());
    delay(10);
  }
  if (serialCommand.length() > 0) {
//    serialCommand.trim();
    if (GetOrder(serialCommand)) {
      radio.openReadingPipe(1, USC);
      radio.startListening(); 
      serialCommand = "";
      
    } else {
      radio.openReadingPipe(1, US1);
      radio.startListening();
      Serial.print("200 command error\r\n");
      serialCommand = "";
    }
  }
  while (radio.available()) {
    radio.read(&rcvMsg, sizeof(rcvMsg));
    command = String(rcvMsg);
  }
  if (command.length() > 0) {
    if(command.startsWith("102")) {
      int num = command.substring(command.indexOf('|')+1, command.indexOf('|')+2).toInt();
      String usState = command.substring(command.indexOf('|')+3, command.indexOf('|')+7);
      if (usState == "TRUE") {
        usStatus[num-1] = 1;
      } else {usStatus[num-1] = 0;}
    }
    Serial.println(command);
    command = "";
  }
}

int GetOrder(String command) {
  command.trim();
  if (command == "HELLO") {
    Serial.print("101 US_PC|1.0|IA2017016785\r\n");
    command = "";
    return 1;
    
  } else if (command.startsWith("US_ON")) {
    int num = command.substring(command.indexOf(' ')+1, command.length()).toInt();
    TransferOrder(command, num);
    Serial.print("100\r\n");
    return 1;
    
  } else if (command.startsWith("US_OFF")) {
    int num = command.substring(command.indexOf(' ')+1, command.length()).toInt();
    TransferOrder(command, num);
    Serial.print("100\r\n");
    return 1;
    
  } else if (command.startsWith("US_SLEEP")) {
    int num = command.substring(command.indexOf(' ')+1, command.length()).toInt();
    TransferOrder(command, num);
    Serial.print("100\r\n");
    return 1;
    
  } else if (command.startsWith("US_WAKEUP")) {
    int num = command.substring(command.indexOf(' ')+1, command.length()).toInt();
    TransferOrder(command, num);
    Serial.print("100\r\n");
    return 1;
    
  } else if (command.startsWith("US_SET")) {
    int num = command.substring(command.indexOf(' ')+1, command.indexOf(',')).toInt();
    TransferOrder(command, num);
    Serial.print("100\r\n");
    return 1; 

  } else if (command.startsWith("US_STATUS")) {
     int num = command.substring(command.indexOf(' ')+1, command.length()).toInt();
     String reply = CheckStatus(num);
     Serial.print(reply+"\r\n");
     return 1;
  }
  else {
    return 0;
    
  }
}

void TransferOrder (String src, int num) {
  uint64_t channel = 0xE6E6E6E6E6FF;
  if (num == 1) {
    channel = 0xE6E6E6E6E677;
  } else if (num == 2) {
    channel = 0xE6E6E6E6E678;
  } else if (num == 3) {
    channel = 0xE6E6E6E6E679;
  }
  char msg[32] = "";
  src.toCharArray(msg, 32);
//  Serial.println(msg);
  radio.stopListening();
  radio.openWritingPipe(channel);
  radio.write(&msg, sizeof(msg));
}

String CheckStatus(int num) {
  if (usStatus[num-1] == 1) {
    return "TRUE";
  } else return "FALSE";
}

