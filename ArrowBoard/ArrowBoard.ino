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
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
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
byte stopSign[][32] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x07,0xE0,0x0F,0xF0,0x0F,0xF0,
0x0F,0xF0,0x0F,0xF0,0x07,0xE0,0x03,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
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
    Display(up, Green);
  } else if(flag == 2) {  //DOWN
    Display(down, Green);
  } else if(flag == 3) {  //LEFT
    Display(left, Green);
//    Display(up, Green);
  } else if(flag == 4) {  //RIGHT
    Display(right,Green);
  } else if(flag == 5) {  //STOP
    Display(stopSign, Red);
  }else if(flag == 0) {   //BLANK
    Display(blank, Green);
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
  } else if(dir == "STOP") {
    flag = 5;
  } else Serial.println("200 command error");
  Serial.println("On"+dir+f);
}

void ArrowOff(String dir) {
  flag = 0;
  Serial.println("Off"+dir);
}

void hc138scan(byte r) {
  digitalWrite(RowA, (r & 0x01));
  digitalWrite(RowB, (r & 0x02));
  digitalWrite(RowC, (r & 0x04));
  digitalWrite(RowD, (r & 0x08));
}

void spi_transfer(volatile char data)   //硬件SPI
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the OEd of the transmission
  {
  };
  //return SPDR;                  // return the received byte
}

void software_transfer(byte data, int color) {
  for(byte i=0; i<8; i++) {
    digitalWrite(clk, 0);
    if(data & 0x08) {
      digitalWrite(color, 0);
    } else {
      digitalWrite(color, 1);
    }
    data = data << 1;
    digitalWrite(clk, 1);
  }
}

void Display(byte data[][32], int color) {
  if(color == Green) {
    digitalWrite(Red, HIGH);
  } else if(color == Red) {
    digitalWrite(Green, HIGH);
  }
  for(int row=0; row<16; row++) {  //循环扫描16行
    for (int i=0; i<1; i++) {
      //硬件SPI  发送8字节耗时:22US         
//      spi_transfer(~(hz[i][row*2]));  //硬件SPI      
//      spi_transfer(~(hz[i][row*2+1]));  //硬件SPI   

      //软件SPI  发送8字节耗时:770US
      software_transfer((data[i][row*2]), color);
      software_transfer((data[i][row*2+1]), color);
      shiftOut(color,clk,MSBFIRST,~data[i][row*2]);
      shiftOut(color,clk,MSBFIRST,~data[i][row*2+1]);
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
