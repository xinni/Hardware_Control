//安装于超声测距板上，通过rf与PC端arduino通讯，接收控制超声波模块的命令
//指令格式： US_ON 打开超声测距，并回传信息 US_OFF 关闭超声测距
//收到打开指令后，开始测距，并回传距离信息，收到关闭指令后关闭超声测距
//回复格式：102 50：表示超音波距離偵測完成。距離為50公分(cm)

#include "RF24.h"
#include "SPI.h"

//超声波模块ID，需要与超声波控制端ID相对应
const uint64_t US1 = 0xE6E6E6E6E677;
const uint64_t USC = 0xE6E6E6E6E666;

float detectSet = 0;

RF24 radio(9,10);
const int TrigPin = 2;
const int EchoPin = 3;
boolean state = false;
int flag = 0;
float cm;
char rcvMsg[32] = "";
char msg[32] = "";

String command = "";
String serialCommand = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(1, US1);
  radio.startListening();
}

void loop() {
/*************************************************************/
/************      Serial receive      ***********************/
/*************************************************************/
  while (Serial.available()) {
    //serialCommand += char(Serial.read());
    serialCommand += char(Serial.read());
    delay(10);
  }
  if (serialCommand.length() > 0) {
    if (serialCommand.startsWith("HELLO")) {
      Serial.print("101 ULTRASOUND|1.0|AR2017016785\r\n");
      serialCommand = "";
    } else {
      Serial.print("200 command error\r\n");
      serialCommand = "";
    }
  }

/*************************************************************/
/*************        RF receive     *************************/
/*************************************************************/
  while (radio.available()) {
    radio.read(&rcvMsg, sizeof(rcvMsg));
    command = String(rcvMsg);
  }
  if (command.length() > 0) {
      Serial.println(command);
      if (GetOrder(command)) {
        command = "";
      } else {
        command = "";
      }
  }

/*************************************************************/
/*************        US detect      *************************/
/*************************************************************/
  if (state & (detectSet != 0)) {
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
//  Serial.println(cm);
  if (cm < detectSet) {
    if (flag == 0) {
      String dist = String("102|1|TRUE|"+(String)cm);
      dist.toCharArray(msg, 32);
      radio.stopListening();
      radio.openWritingPipe(USC);
      Serial.println(dist);
      radio.write(&msg, sizeof(msg));
      flag = 1;
    }
  } else {
    if (flag == 1) {
      String dist = String("102|1|FALSE|"+(String)cm);
      dist.toCharArray(msg, 32);
      radio.stopListening();
      radio.openWritingPipe(USC);
      Serial.println(dist);
      radio.write(&msg, sizeof(msg));
      flag = 0;
    }
  }
  radio.openReadingPipe(1, US1);
  radio.startListening();
  delay(1); 
}

int GetOrder(String command) {
  if (command.startsWith("US_ON")) {
    state = true;
    command = "";
    return 1;

  } else if (command.startsWith("US_OFF")) {
    state = false;
    command = "";
    return 1;

  } else if (command.startsWith("US_SET")) {
    String num = command.substring(command.indexOf(' ')+1, command.indexOf(','));
    String set = command.substring(command.indexOf(',')+1, command.length());
    detectSet = set.toInt();
    command = "";
    return 1;

  } else if (command.startsWith("US_SLEEP")) {
    command = "";
    return 1;

  } else if (command.startsWith("US_WAKEUP")) {
    command = "";
    return 1;

  }

  else return 0;
}
