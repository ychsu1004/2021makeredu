// This post referred to this git. I just trimmed cam and wifi part.
// https://github.com/v12345vtm/CameraWebserver2SD/blob/master/CameraWebserver2SD/CameraWebserver2SD.ino

#include "FS.h" 
#include "SD_MMC.h" 

String buffer;

//Read a file in SD card
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("Read from file: ");
    while(file.available()){
      buffer = file.readStringUntil('\n');
      Serial.println(buffer); //Printing for debugging purpose   
      //Serial.write(file.read());
    }
    file.close();
    if (buffer == "布氏指數2"){
      Serial.println("繼電器啟動");
    }
    
}

void setup() {  
  Serial.begin(115200);
  Serial.println("SDcard Testing....");

   if(!SD_MMC.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

    readFile(SD_MMC, "/foo.txt"); 
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
