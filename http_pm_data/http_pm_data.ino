#include <WiFi.h>//wifi
#include <WiFiMulti.h> // 多重連線
#include <HTTPClient.h> // 瀏覽器
#include <ArduinoJson.h> // 使用JSON 函式庫
char ssid[] = "SSID"; // 請修改為您連線的網路名稱
char password[] = "SSIDpassword"; // 請修改為您連線的網路密碼
char url[] = "http://opendata2.epa.gov.tw/AQI.json"; // 讀取的網址
void setup() {
Serial.begin(115200);
delay(1000);
Serial.print("開始連線到無線網路SSID:");
Serial.println(ssid);
// 1. 設定WiFi 模式
WiFi.mode(WIFI_STA);
// 2. 啟動WiFi 連線
WiFi.begin(ssid, password);
// 3. 檢查連線狀態
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(1000);
}
Serial.println("連線完成");
}
void loop() {
// 4. 啟動網頁連線
Serial.println("啟動網頁連線");
HTTPClient http;
http.begin(url);
int httpCode = http.GET();
Serial.print("httpCode=");
Serial.println(httpCode);
// 5. 檢查網頁連線是否正常
if (httpCode == HTTP_CODE_OK) {
// 6. 取得網頁內容
String payload = http.getString();
Serial.print("payload=");
// 7. 將資料顯示在螢幕上
Serial.println(payload);
// JSON 格式解析
DynamicJsonDocument AQJarray(payload.length() * 2);
deserializeJson(AQJarray, payload); // 解析payload 為JSON Array 格式
for (int i = 0; i <= AQJarray.size(); i++) {
if (AQJarray[i]["SiteId"] == "48") {
// 找到需要橋頭站Siteid=48
String KSAQI=AQJarray[i]["AQI"];
Serial.println("橋頭AQI=" + KSAQI);
}
http.end();
delay(10000);
}
}
