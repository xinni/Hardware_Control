#define RowA 2
#define RowB 3
#define RowC 4
#define RowD 5
#define LATCH 10

int latch = 10;
int hc138en = 6;
int clk = 9;
int Red = 8; //R1
int Green = 7; //G1

int flag = 0;

//byte row = 0;
byte blank[][32] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //尾部的一屏的空字符，用于清屏，参数和屏大小有关。
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
};
byte up[][32] = {
/*--  Up   --*/
/*--  宽x高=16x16   --*/
0x01,0x80,0x03,0xC0,0x07,0xE0,0x0F,0xF0,0x1F,0xF8,0x3F,0xFC,0x7F,0xFE,0xFF,0xFF,
0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,
};
byte down[][32] = {
/*--  Down   --*/
/*--  宽x高=16x16   --*/
0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,
0xFF,0xFF,0x7F,0xFE,0x3F,0xFC,0x1F,0xF8,0x0F,0xF0,0x07,0xE0,0x03,0xC0,0x01,0x80,
};
byte left[][32] = {
/*--  Left   --*/
/*--  宽x高=16x16   --*/
0x01,0x00,0x03,0x00,0x07,0x00,0x0F,0x00,0x1F,0x00,0x3F,0xFF,0x7F,0xFF,0xFF,0xFF,
0xFF,0xFF,0x7F,0xFF,0x3F,0xFF,0x1F,0x00,0x0F,0x00,0x07,0x00,0x03,0x00,0x01,0x00,
};
byte right[][32] = {
/*--  Right   --*/
/*--  宽x高=16x16   --*/
0x00,0x80,0x00,0xC0,0x00,0xE0,0x00,0xF0,0x00,0xF8,0xFF,0xFC,0xFF,0xFE,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFE,0xFF,0xFC,0x00,0xF8,0x00,0xF0,0x00,0xE0,0x00,0xC0,0x00,0x80,
};


String command = "";

void setup() {
  pinMode(RowA, OUTPUT);
  pinMode(RowB, OUTPUT);
  pinMode(RowC, OUTPUT);
  pinMode(RowD, OUTPUT);
  pinMode(hc138en, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(clk, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  
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
  if(flag == 1) {   //UP
    digitalWrite(Red, HIGH);
    Display(up);
  } else if(flag == 2) {
    digitalWrite(Red, HIGH);
    Display(down);
  } else if(flag == 3) {
    digitalWrite(Red, HIGH);
    Display(left);
  } else if(flag == 4) {
    digitalWrite(Red, HIGH);
    Display(right);
  } else if(flag == 0) {
    digitalWrite(Red, HIGH);
    Display(blank);
  }

}

int GetOrder(String command) {
  if (command.startsWith("AR_ON")) {
    String dir = command.substring(command.indexOf(' ')+1, command.indexOf(','));
//  dir is the direction of the arrow
    String f = command.substring(command.indexOf(',')+1, command.length());
    ArrowOn(dir, f);
    command = "";
    return 1;
    
  } else if (command.startsWith("AR_OFF")) {
    String dir = command.substring(command.indexOf(' ')+1, command.length());
    ArrowOff(dir);
    command = "";
    return 1;
  } 
  
  else return 0;
}

void ArrowOn(String dir, String f) {
  if(dir == "UP") {
    flag = 1;
  } else if(dir == "DOWN") {
    flag = 2;
  } else if(dir == "LEFT") {
    flag = 3;
  } else if(dir == "RIGHT") {
    flag = 4;
  }
  Serial.print("On"+dir+f);
}

void ArrowOff(String dir) {
  Serial.print("Off"+dir);
}

void hc138scan(byte r) {
  digitalWrite(RowA, (r & 0x01));
  digitalWrite(RowB, (r & 0x02));
  digitalWrite(RowC, (r & 0x04));
  digitalWrite(RowD, (r & 0x08));
}

void software_transfer(byte data) {
  for(byte i=0; i<2; i++) {
    digitalWrite(clk, 0);
    if(data & 0x08) {
      digitalWrite(Green, 0);
    } else {
      digitalWrite(Green, 1);
    }
    data = data << 1;
    digitalWrite(clk, 1);
  }
}

//void spi_transfer(volatile char data) {
//  SPDR = data;
//  while (!SPSR & (1 << SPIF));
//}

//Data in Serial transfer
//void SendByte(unsigned char dat) {
//  static unsigned char i;
//  for (i = 0; i< 8; i++) {
//    digitalWrite(Red,bitRead(dat, 7-i));
//    digitalWrite(clk, 0);
//    digitalWrite(clk, 1);
//  }
//}
//
//void SendByte2(unsigned char dat, byte offset) {
//  static unsigned char i;
//  for(i = 0; i < offset; i++) {
//    digitalWrite(Red, bitRead(dat, 7-i));
//    digitalWrite(clk, 0);
//    digitalWrite(clk, 1);
//  }
//}

void Display(byte data[][32]) {
  for(int row=0; row<16; row++) {  //循环扫描16行
    for (int i=0; i<1; i++) {
      //硬件SPI  发送8字节耗时:22US         
//      spi_transfer(~(hz[i][row*2]));  //硬件SPI      
//      spi_transfer(~(hz[i][row*2+1]));  //硬件SPI   

      //软件SPI  发送8字节耗时:770US
      software_transfer((data[i][row*2]));
      software_transfer((data[i][row*2+1]));
      shiftOut(Green,clk,MSBFIRST,~data[i][row*2]);
      shiftOut(Green,clk,MSBFIRST,~data[i][row*2+1]);
    }
    digitalWrite(hc138en, 1);    //关闭显示
    hc138scan(15 - row);         //换行
    digitalWrite(LATCH, 0);      //595刷新
    digitalWrite(LATCH, 1);
    //delayMicroseconds(500) ;   //节电用
    digitalWrite(hc138en, 0);    //开启显示
    //delayMicroseconds(10) ;    //刷新频率调,差不多60HZ,1/16间隔
  }
}
