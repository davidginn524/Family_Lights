#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const char* mqttServer = "m23.cloudmqtt.com";
const int mqttPort = 16743;
const char* mqttUser = "crjhzcfb";
const char* mqttPassword = "OgX58L0IebEa";
 
WiFiClient espClient;
PubSubClient client(espClient);


#define PIXEL_PIN       4                      // Pin connected to the NeoPixel data input.
#define PIXEL_COUNT     21                      // Number of NeoPixels.
#define PIXEL_TYPE      NEO_GRB + NEO_KHZ800   // Type of the NeoPixels (see strandtest example).
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE); // create NeoPixels object



void setup() {
   // Initialize NeoPixels and turn them off.
  pixels.begin();
  
  Serial.begin(115200);
  
  // We start by connecting to a WiFi network
  WiFiManager MyWifiManager;
  MyWifiManager.autoConnect("Family_Lamps");
//  WiFi.begin(ssid, password);
//  Serial.println("Connecting to WiFi..");
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("Connected to the WiFi network");
// 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  
  client.publish("Family_Lamps/Incoming_Colors", "Hello from ESP8266");
  client.subscribe("Family_Lamps/Incoming_Colors");
  


}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
 
void loop() {
  client.loop();
}
