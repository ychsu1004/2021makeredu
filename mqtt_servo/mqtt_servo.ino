#include <WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
// -------- 伺服馬達控制---------
#include <ESP32Servo.h>
Servo myServo; // 建立一個伺服馬達物件
// ------ 以下修改成你自己的WiFi 帳號密碼 ------
char ssid[]="SSID";
char password[]="SSIDpassword";
//------ 以下修改成你DHT11 腳位 ------
int pinDHT11 = 13;
SimpleDHT11 dht11(pinDHT11);
// ------ 以下修改成你MQTT 設定 ------
char* MQTTServer = "mqtt.eclipseprojects.io"; // 免註冊MQTT 伺服器
int MQTTPort = 1883; // MQTT Port
char* MQTTUser = ""; // 不須帳密
char* MQTTPassword = ""; // 不須帳密
// 推播主題1: 推播溫度（記得改Topic）
char* MQTTPubTopic1 = "YourTopic/class205/temp";
// 推播主題2: 推播濕度（記得改Topic）
char* MQTTPubTopic2 = "YourTopic/class205/humi";
// 訂閱主題1: 改變伺服馬達角度（記得改Topic）
char* MQTTSubTopic1 = "YourTopic/class205/servo";
long MQTTLastPublishTime; // 此變數用來記錄推播時間
long MQTTPublishInterval = 10000; // 每10 秒推播一次
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);
void setup() {
Serial.begin(115200);
myServo.attach(4, 500, 2400); // 伺服馬達
// 開始WiFi 連線
WifiConnecte();
// 開始MQTT 連線
MQTTConnecte();
}
void loop() {
  // 如果WiFi 連線中斷，則重啟WiFi 連線
if (WiFi.status() != WL_CONNECTED) {
WifiConnecte();
}
// 如果MQTT 連線中斷，則重啟MQTT 連線
if (!MQTTClient.connected()) {
MQTTConnecte();
}
// 如果距離上次傳輸已經超過10 秒，則Publish 溫濕度
if ((millis() - MQTTLastPublishTime) >= MQTTPublishInterval ) {
// 讀取溫濕度
byte temperature = 0;
byte humidity = 0;
ReadDHT(&temperature, &humidity);
// ------ 將DHT11 溫度送到MQTT 主題 ------
MQTTClient.publish(MQTTPubTopic1, String(temperature).c_str());
MQTTClient.publish(MQTTPubTopic2, String(humidity).c_str());
Serial.println("溫濕度已推播到MQTT Broker");
MQTTLastPublishTime = millis(); // 更新最後傳輸時間
}
MQTTClient.loop(); // 更新訂閱狀態
delay(50);
}
// 讀取DHT11 溫濕度
void ReadDHT(byte *temperature, byte *humidity) {
int err = SimpleDHTErrSuccess;
if ((err = dht11.read(temperature, humidity, NULL)) !=
SimpleDHTErrSuccess) {
Serial.print("讀取失敗, 錯誤訊息=");
Serial.print(SimpleDHTErrCode(err));
Serial.print(","); Serial.println(SimpleDHTErrDuration(err));
delay(1000);
return;
}
Serial.print("DHT 讀取成功：");
Serial.print((int)*temperature); Serial.print(" *C, ");
Serial.print((int)*humidity); Serial.println(" H ");
}
// 開始WiFi 連線
void WifiConnecte() {
// 開始WiFi 連線
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("WiFi 連線成功");
Serial.print("IP Address:");
Serial.println(WiFi.localIP());
}
// 開始MQTT 連線
void MQTTConnecte() {
MQTTClient.setServer(MQTTServer, MQTTPort);
MQTTClient.setCallback(MQTTCallback);
while (!MQTTClient.connected()) {
// 以亂數為ClietID
String MQTTClientid = "esp32-" + String(random(1000000, 9999999));
if (MQTTClient.connect(MQTTClientid.c_str(), MQTTUser,
MQTTPassword)) {
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
  Serial.print(topic);
Serial.print("訂閱通知:");
String payloadString; // 將接收的payload 轉成字串
// 顯示訂閱內容
for (int i = 0; i < length; i++) {
payloadString = payloadString + (char)payload[i];
}
Serial.println(payloadString);
// 比對主題是否為訂閱主題1
if (strcmp(topic, MQTTSubTopic1) == 0) {
Serial.println("開啟飼料機：" + payloadString + "ms");
myServo.write(30); // 轉到30 度（開啟飼料機）
// 將資料轉成整數後，設定為開啟時間
int opentime=payloadString.toInt();
delay(opentime); // 開啟時間
myServo.write(0); // 轉到0 度（關閉飼料機）
}
}
