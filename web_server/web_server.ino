#include <WiFi.h>
// 請修改以下參數--------------------------------------------
char ssid[] = "TP-Link_B4D0";
char password[] = "0937945797";
int Gled = 12; // 宣告綠色Led 在 GPIO 4
WiFiServer server(80); // 宣告伺服器位在80 port

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
    client.println("<a href='/Gled=ON'> 開啟綠色LED</a><br>");
    client.println("<a href='/Gled=OFF'> 關閉綠色LED</a><br>");
    client.println("</html>");
    // ------------- 網頁的html 部分結束--------------
    // 取得使用者輸入的網址
    String request = client.readStringUntil('\r');
    Serial.println(request);
    // 判斷超連結指令
    // 網址內包含Gled=ON，就開啟綠燈，如果Gled=OFF，關閉綠燈
    if (request.indexOf("Gled=ON") >= 0) {
      digitalWrite(Gled, HIGH); 
    }
    if (request.indexOf("Gled=OFF") >= 0){
      Serial.println("B");
      digitalWrite(Gled, LOW);
    }
    Serial.println("完成");
    client.stop(); // 停止連線
   } 
}

void setup(){
  Serial.begin(115200);
  Serial.print("開始連線到無線網路SSID:");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("連線完成");
  server.begin();
  Serial.print("伺服器已啟動，http://");
  Serial.println(WiFi.localIP());
  pinMode(Gled, OUTPUT);
}

void loop(){
  //開啟伺服器
  webserver();
}
