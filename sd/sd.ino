/*******************************************************
   ESP32 LED点阵显示信息
   功能：使用SPI LED点阵显示信息
   引脚：GPIO26 -> CS  GPIO14 -> MOSI（DI） GPIO12 -> MISO（DO） GPIO27 - SCK 
   Designer: Code_Mouse
   Date:2018-9-8
 *******************************************************/
#include <mySD.h>
File root;
void setup()
{
  Serial.begin(115200);
 
  Serial.print("Initializing SD card...");
/*初始化SD库SPI引脚*/ 
  if (!SD.begin(5, 23, 19, 18)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
/*从根目录root“/”*/ 
  root = SD.open("/");
  if (root) {    
    printDirectory(root, 0);
    root.close();
  } else {
    Serial.println("error opening test.txt");
  }
 /*打开“test.txt”写入*/ 
  root = SD.open("test.txt", FILE_WRITE);
 /*如果成功打开 - > root！= NULL 然后写字符串“Hello world！”*/ 
  if (root) {
    root.println("Hello world!");
    root.flush();
   /*关闭文件 */
    root.close();
  } else {
    /* 如果文件打开错误，则打印错误 */
    Serial.println("error opening test.txt");
  }
  delay(1000);
  /*写完后再重新打开文件并读取它 */
  root = SD.open("test.txt");
  if (root) {    
    /* 从文件中读取，直到其中没有其他内容 */while (root.available()) {
      /* 读取文件并打印到串口监视器*/
      Serial.write(root.read());
    }
    root.close();
  } else {
    Serial.println("error opening test.txt");
  }
  
  Serial.println("done!");
}
 
void loop()
{
}
