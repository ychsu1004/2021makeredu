#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SD_MMC.h" 
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <TridentTD_LineNotify.h>

char ssid[] = "kevinAP"; // 連線的網路名稱
char password[] = "035532759"; // 連線的網路密碼
String Linetoken = "cJNBeWgkKh9LlKS51Wr29vWcGVk7ockzkSHQWSGYloP"; //Line權杖

// ------ ESP32使用腳位 -------
int get_D3 = 39;      //雨滴偵測器腳位
int get_A0 = 34;      //雨滴偵測器腳位
int get_water = 36;   //水位偵測器腳位
int RelayPin = 22;    //繼電器腳位

// ------ 參數初始設定 -------
int water_level = 0;
int rain_level = 0;
int water_value = 0;
int rain_value = 0;
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor
int rain_line_message_count = 0;
int water_line_message_count = 0;
String buffer;

// ------ MQTT 設定 -------
//char* MQTTServer = "mqtt.eclipseprojects.io"; // 免註冊MQTT 伺服器
char* MQTTServer = "broker.hivemq.com"; // 免註冊MQTT 伺服器
int MQTTPort = 1883; // MQTT Port
char* MQTTUser = ""; // 不須帳密
char* MQTTPassword = ""; // 不須帳密
// 推播主題1: 推播雨量
char* MQTTPubTopic1 = "dxjh/class/rain";
// 推播主題2: 推播水位
char* MQTTPubTopic2 = "dxjh/class/water";
// 訂閱主題1: 改變繼電器燈號
char* MQTTSubTopic1 = "dxjh/class/light";
long MQTTLastPublishTime; // 此變數用來記錄推播時間
long MQTTPublishInterval = 5000; // 每10秒推撥一次
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);


//char host[] = "notify-api.line.me"; // 宣告Line API 網站
// ------ 網頁伺服器 設定 -------
WiFiServer server(80); // 宣告伺服器位在80 port
WiFiClientSecure client; // 宣告一個加密的WiFi 連線端

//------ line訊息 -------
String rain_message = "下雨了，請清理家裡積水容器!!";
String water_message = "容器已積水，請清理家裡積水容器!!";
String message = "家中滅蚊燈啟動!!";

//SD卡資料讀取
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
      Serial.println(buffer);
      //Serial.write(file.read());
    }
    file.close();
    if (buffer == "布氏指數2"){
      Serial.println("滅蚊燈啟動");
      digitalWrite(RelayPin, HIGH);
      // 顯示 Line版本
      Serial.println(LINE.getVersion());
      LINE.setToken(Linetoken);
      // 先換行再顯示
      LINE.notify("\n" + message);
    }
}

// 開始WiFi 連線
void WifiConnecte() {
  // 開始WiFi 連線
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("WiFi 連線成功");
  server.begin();
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}

void webserver() {
   // 宣告一個連線
  WiFiClient client = server.available();
  if (client) {
  // 有人連入時
    Serial.println("使用者連入");
    //------------- 網頁的html 部分開始--------------
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html><head><meta charset='utf-8'></head>");
    client.println("<br>");
    client.println("<h1>ESP32 Web Server</h1>");
    // HTML 超連結指令
    client.println("<a href='/Gled=ON'> 開啟滅蚊燈</a><br>");
    client.println("<a href='/Gled=OFF'> 關閉滅蚊燈</a><br>");
    client.println("</html>");
    // ------------- 網頁的html 部分結束--------------
    // 取得使用者輸入的網址
    String request = client.readStringUntil('\r');
    Serial.println(request);
    // 判斷超連結指令
    // 網址內包含Gled=ON，就開啟，如果Gled=OFF，關閉
    if (request.indexOf("Gled=ON") >= 0) {
      digitalWrite(RelayPin, HIGH);
      // 顯示 Line版本
      Serial.println(LINE.getVersion());
      LINE.setToken(Linetoken);
      // 先換行再顯示
      LINE.notify("\n" + message);
    }
    if (request.indexOf("Gled=OFF") >= 0){
      digitalWrite(RelayPin, LOW);
    }
    Serial.println("完成");
    client.stop(); // 停止連線
   } 
}

// 開始MQTT 連線
void MQTTConnecte() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  MQTTClient.setCallback(MQTTCallback);
  while (!MQTTClient.connected()) {
    // 以亂數為ClietID
    String MQTTClientid = "esp32-" + String(random(1000000, 9999999));
    if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser, MQTTPassword)) {
      // 連結成功，顯示「已連線」。
      Serial.println("MQTT 已連線");
      // 訂閱SubTopic1 主題
      MQTTClient.subscribe(MQTTSubTopic1);
    } else {
      // 若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print("MQTT 連線失敗, 狀態碼=");
      Serial.println(MQTTClient.state());
      Serial.println("五秒後重新連線");
      delay(5000);
    }
  }
}

// 接收到訂閱時
void MQTTCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic); Serial.print("訂閱通知:");
  String payloadString; // 將接收的payload 轉成字串
  // 顯示訂閱內容
  for (int i = 0; i < length; i++) {
    payloadString = payloadString + (char)payload[i];
  }
  Serial.println(payloadString);
  // 比對主題是否為訂閱主題1
  if (strcmp(topic, MQTTSubTopic1) == 0) {
    Serial.println("改變繼電器燈號：" + payloadString);
    if (payloadString == "ON") {
      digitalWrite(RelayPin, HIGH);
      // 顯示 Line版本
      Serial.println(LINE.getVersion());
      LINE.setToken(Linetoken);
      // 先換行再顯示
      LINE.notify("\n" + message);
     }
    if (payloadString == "OFF") {
      digitalWrite(RelayPin, LOW);
    }
  }
}

void water() {
  water_level = analogRead(get_water); //從P36讀取輸入
  Serial.print(water_level);    
  Serial.print("   ");      
  if (water_level > 500) {
    water_value = 1;
    Serial.println("High Level");
  } else {
    water_value = 0;
    Serial.println("Low Level");     
  }
}

void rain() {
  rain_level = analogRead(get_A0);    //從A0讀取感測值
 // Serial.print(rain_level);    
  if(digitalRead(get_D3) == LOW){
    rain_value = 1;
    Serial.println("Digital value : wet");   
  }
  else{
    rain_value = 0;
    Serial.println("Digital value : dry");
  }
  delay(1000); 
}
void SD_CARD() {
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
}

void setup() {
    Serial.begin(115200);
    
    // 開始WiFi 連線
    WifiConnecte();

    // 開始MQTT 連線
    MQTTConnecte();

    //SD卡
    SD_CARD();    
   
    pinMode(get_D3, INPUT);
    pinMode(RelayPin, OUTPUT);
    digitalWrite(RelayPin, LOW); //預設繼電器關閉
    readFile(SD_MMC, "/foo.txt");
}

void loop() {
  // 如果WiFi 連線中斷，則重啟WiFi 連線
  if (WiFi.status() != WL_CONNECTED) {
   WifiConnecte();
  }
  
  // 量測間等待至少 2 秒
  //unsigned long currentMillis = millis(); 
  //if(currentMillis - previousMillis >= interval) {
    // 將最後讀取感測值的時間紀錄下來 
    //previousMillis = currentMillis; 
  //}
  
  //開啟伺服器
  webserver();
  
  // 如果MQTT 連線中斷，則重啟MQTT 連線
  if (!MQTTClient.connected()) {
    MQTTConnecte();
  }
  
  // 如果距離上次傳輸已經超過10 秒，則Publish
 if ((millis() - MQTTLastPublishTime) >= MQTTPublishInterval ) {
    // ------ 將資料送到MQTT 主題 ------
    if (rain_value == 1) {
      MQTTClient.publish(MQTTPubTopic1, rain_message.c_str());
      Serial.println("雨水訊息已推播到MQTT Broker");
    }
    if (water_value == 1) {
      MQTTClient.publish(MQTTPubTopic2, water_message.c_str());   
      Serial.println("水位訊息已推播到MQTT Broker"); 
    }
    rain();//雨量偵測
    water();//水位偵測
    if (rain_value == 1 and rain_line_message_count >= 10) { //設定雨量偵測觸發LINE 訊息條件
      Serial.println(LINE.getVersion()); // 顯示 Line版本
      LINE.setToken(Linetoken);
      // 先換行再顯示
      LINE.notify("\n" + rain_message); // 先換行再顯示
      rain_line_message_count++;
    }

    if (water_value == 1 and water_line_message_count >= 10) { //設定水位偵測觸發LINE 訊息條件
      Serial.println(LINE.getVersion()); // 顯示 Line版本
      LINE.setToken(Linetoken);
      // 先換行再顯示
      LINE.notify("\n" + water_message); // 先換行再顯示
      water_line_message_count++;
    }
    
    MQTTLastPublishTime = millis(); // 更新最後傳輸時間
  }
  
  MQTTClient.loop(); // 更新訂閱狀態
  
}
