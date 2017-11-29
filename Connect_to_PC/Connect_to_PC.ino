//通过串口与PC通信(Baud rate:115200),接收PC端开关红外线灯板的命令
//指令格式：IR_ON 2,3：表示打开2号灯板，3级亮度  IR_OFF 2：关闭2号灯板
//没有回复

#include "RF24.h"
#include "SPI.h"

//灯板程序中的ID需要与下面的ID相对应
const uint64_t LIGHT1 = 0xE6E6E6E6E601;
const uint64_t LIGHT2 = 0xE6E6E6E6E601;
const uint64_t LIGHT3 = 0xE6E6E6E6E603;
const uint64_t LIGHT4 = 0xE6E6E6E6E603;
const uint64_t LOGO = 0xE6E6E6E6E677;

RF24 radio(9,10);
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
      Serial.println("200 command error");
      command = "";
    }
  }
}

int GetOrder(String command) {
  if (command.startsWith("IR_ON")) {
    String number = command.substring(command.indexOf(' ')+1, command.indexOf(','));
    int num = number.toInt();
//  num is the number of the light
    String brightness = command.substring(command.indexOf(',')+1, command.indexOf('\r'));
    int brightNum = brightness.toInt();
//  brightNum is the brightness of the light
    TurnOn(num, brightNum);
    command = "";
    return 1;
    
  } else if (command.startsWith("IR_OFF")) {
    String number = command.substring(command.indexOf(' ')+1, command.indexOf('\r'));
    int num = number.toInt();
    TurnOff(num);
    command = "";
    return 1;
  } else if (command == "HELLO") {
    Serial.println("101 IRLAMP_ARROW|1.0|IA2017016785");
    command = "";
    return 1;
    
  } else if (command.startsWith("AR_ON")) {
    String dir = command.substring(command.indexOf(' ')+1, command.indexOf(','));
//  dir is the direction of the arrow
    String f = command.substring(command.indexOf(',')+1, command.indexOf('\r'));
    TransferOrder(command);
    command = "";
    return 1;
    
  } else if (command.startsWith("AR_OFF")) {
    String dir = command.substring(command.indexOf(' ')+1, command.indexOf('\r'));
    TransferOrder(command);
    command = "";
    return 1;
    
  } else if (command.startsWith("LOGO_ON")) {
    String color = command.substring(command.indexOf(' ')+1, command.indexOf('\r'));
    TransferOrder(command);
    command = "";
    return 1;
    
  } else if (command.startsWith("GREEN_ON")) {
    String pos = command.substring(command.indexOf(' ')+1, command.indexOf('\r'));
    TransferOrder(command);
    
  } else if (command.startsWith("RED_ON")) {
    String pos = command.substring(command.indexOf(' ')+1, command.indexOf('\r'));
    TransferOrder(command);
    
  } else if (command.startsWith("POS_OFF")) {
    String pos = command.substring(command.indexOf(' ')+1, command.indexOf('\r'));
    TransferOrder(command);
  }
  
  else return 0;
}

void TurnOn (int num, int bright) {
  char msg[32] = "";
  if ((bright >= 1) && (bright <= 8)) {
    if (num == 1) {
      sendMessage = String("ON,1|"+String(bright)); 
      sendMessage.toCharArray(msg, 32);
      radio.openWritingPipe(LIGHT1);
      radio.write(&msg, sizeof(msg));
      Serial.println("100");
    
    } else if (num == 2) {
      sendMessage = String("ON,2|"+String(bright));
      sendMessage.toCharArray(msg, 32);
      radio.openWritingPipe(LIGHT2);
      radio.write(&msg, sizeof(msg));
      Serial.println("100");
    
    } else if (num == 3) {
      sendMessage = String("ON,1|"+String(bright));
      sendMessage.toCharArray(msg, 32);
      radio.openWritingPipe(LIGHT3);
      radio.write(&msg, sizeof(msg));
      Serial.println("100");
    
    } else if (num == 4) {
      sendMessage = String("ON,2|"+String(bright));
      sendMessage.toCharArray(msg, 32);
      radio.openWritingPipe(LIGHT4);
      radio.write(&msg, sizeof(msg));
      Serial.println("100");
    
    } else Serial.println("201 "+String(num)+"|"+String(bright)+"|Open IR lamp fail");
  } else Serial.println("201 "+String(num)+"|"+String(bright)+"|Open IR lamp fail");
  sendMessage = "";
}

void TurnOff (int num) {
  
//  char msg[32] = "";
  if (num == 1) {
    char msg[32] = "OFF,1";
    radio.openWritingPipe(LIGHT1);
    radio.write(&msg, sizeof(msg));
    Serial.println("100");
    
  } else if (num == 2) {
    char msg[32] = "OFF,2";
    radio.openWritingPipe(LIGHT2);
    radio.write(&msg, sizeof(msg));
    Serial.println("100");
    
  } else if (num == 3) {
    char msg[32] = "OFF,1";
    radio.openWritingPipe(LIGHT3);
    radio.write(&msg, sizeof(msg));
    Serial.println("100");
    
  } else if (num == 4) {
    char msg[32] = "OFF,2";
    radio.openWritingPipe(LIGHT4);
    radio.write(&msg, sizeof(msg));
    Serial.println("100");
    
  } else Serial.println("202 "+String(num)+"|Close IR lamp fail");
  sendMessage = "";
}

void TransferOrder (String src) {
  char msg[32] = "";
  src.toCharArray(msg, 32);
  Serial.println(msg);
  radio.openWritingPipe(LOGO);
  radio.write(&msg, sizeof(msg));
}

