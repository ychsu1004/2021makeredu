#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SimpleDHT.h>
// 請修改以下參數--------------------------------------------
char SSID[] = "你的WiFi SSID";
char PASSWORD[] = "你的WiFi 密碼";
String Linetoken = "cJNBeWgkKh9LlKS51Wr29vWcGVk7ockzkSHQWSGYloP";
int pinDHT11 = 14; // 假設DHT11 接在腳位GPIO14，麵包板左側序號8
//---------------------------------------------------------
SimpleDHT11 dht11(pinDHT11);
WiFiClientSecure client; // 宣告一個加密的WiFi 連線端
char host[] = "notify-api.line.me"; // 宣告Line API 網站
byte temperature = 0;
byte humidity = 0;

void line() {
   // 組成Line 訊息
    String message = "檢測環境發生異常，請協助儘速派人查看處理，目前環境狀態：";
    message += "\r\n 溫度=" + String((int)temperature) + "C";
    message += "\r\n 濕度=" + String((int)humidity) + "H";
    Serial.println(message);
    // 連線到Line API 網站
    if (client.connect(host, 443)) {
      int LEN = message.length();
      //(1) 傳遞網站
      String url = "/api/notify"; // Line API 網址
      client.println("POST " + url + "HTTP/1.1");
      client.print("Host: "); client.println(host); // Line API 網站
      //(2) 資料表頭
      client.println("Authorization: Bearer " + Linetoken);
      //(3) 內容格式
      client.println("Content-Type: application/x-www-form-urlencoded");
      //(4) 資料內容
      client.println("Content-Length: " + String((LEN + 8)) ); // 訊息長度
      client.println();
      client.println("message=" + message); // 訊息內容
      // 等候回應
      delay(2000);
      String response = client.readString();
      // 顯示傳遞結果
      Serial.println(response);
      client.stop(); // 斷線，否則只能傳5 次
    } else {
    // 傳送失敗
      Serial.println("connected fail");
    }
}

void setup() {
  Serial.begin(115200);
  // 連線到指定的WiFi SSID
  Serial.print("Connecting Wifi: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // 連線成功，顯示取得的IP
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void loop() {
  // 嘗試讀取溫濕度內容
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
    return;
  }
  // 讀取成功，將溫濕度顯示在序列視窗
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println("H");
  // 設定觸發LINE 訊息條件為溫度超過35 或濕度超過80
  if ((int)temperature >= 35 || (int)humidity >= 80) {
    line();  
  }
  // 每5 秒讀取一次溫濕度
  delay(5000);
}
