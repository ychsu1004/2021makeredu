/****************************************************/
//#include "ESP8266WiFi.h"
#include "WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

/************************* LED *********************************************/
#define GPIO_LED    23
//#define B_LED    0
//#define R_LED    4

/*********************** DHT11 **********************************************/

#define DHTPIN 5 // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

/************************* WiFi Access Point *********************************/

#define WLAN_SSID  "TP-Link_B4D0"
#define WLAN_PASS  "0937945797"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883 // use 1883 for SSL
#define AIO_USERNAME "ychsu"
#define AIO_KEY "aio_Fahs15SpkiXY2BzI3lHaUaLiWG4o"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: /feeds/
//Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
//Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
Serial.begin(115200);
//dht.begin();
delay(10);

Serial.println(F("Adafruit MQTT demo"));

// Connect to WiFi access point.
Serial.println(); Serial.println();
Serial.print("Connecting to ");
Serial.println(WLAN_SSID);

WiFi.begin(WLAN_SSID, WLAN_PASS);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println();

Serial.println("WiFi connected");
Serial.println("IP address: "); Serial.println(WiFi.localIP());

// Setup MQTT subscription for onoff feed.
mqtt.subscribe(&onoffbutton);
pinMode(GPIO_LED, OUTPUT);
//digitalWrite(GPIO_LED, HIGH);
//digitalWrite(B_LED, HIGH);
//digitalWrite(R_LED, HIGH);

}

//uint32_t x=0;
//uint32_t y=0;
void loop() {
// Ensure the connection to the MQTT server is alive (this will make the first
// connection and automatically reconnect when disconnected). See the MQTT_connect
// function definition further below.
  MQTT_connect();

// this is our 'wait for incoming subscription packets' busy subloop
// try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  Serial.println("A");
  while ((subscription = mqtt.readSubscription(5000))) {
    Serial.println("B");
      if (subscription == &onoffbutton) {
        Serial.print(F("Got: "));
        Serial.println((char *)onoffbutton.lastread);
        if( *onoffbutton.lastread == '1') {
            digitalWrite(GPIO_LED, LOW);
        }
        else if( *onoffbutton.lastread == '0') {
          digitalWrite(GPIO_LED, HIGH);
        }
      }
    }

// Now we can publish stuff!
/*float h = dht.readHumidity();
Serial.print(F("\nHumidity : "));
Serial.print(h);
Serial.print("%");
if (! Humidity.publish(h)) {
Serial.println(F("\nSending Humidty is Failed"));
} else {
Serial.println(F("\nSending Humidty is OK!"));
}

//
float t = dht.readTemperature();
float hic = dht.computeHeatIndex(t, h, false);
Serial.print(F("\nTemperature : "));
Serial.print(hic);
Serial.print("*C");
if (! Temperature.publish(hic)) {
Serial.println(F("\nSending Temperature is Failed"));
} else {
Serial.println(F("\nSending Temperature is OK!"));
}
*/
// ping the server to keep the mqtt connection alive
// NOT required if you are publishing once every KEEPALIVE seconds
/*
if(! mqtt.ping()) {
mqtt.disconnect();
}
*/
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

// Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0) {
    // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
