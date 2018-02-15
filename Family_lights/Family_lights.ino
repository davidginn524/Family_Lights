#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

//libraries for auto wifi setup
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

//mqtt server variables 
const char* mqttServer = "m23.cloudmqtt.com";
const int mqttPort = 16743;
const char* mqttUser = "crjhzcfb";
const char* mqttPassword = "OgX58L0IebEa";
 
WiFiClient espClient;
PubSubClient client(espClient);
//cleint ID for hashing and debuging 
const char* CELINT_ID = "1";

  
//define button pin and setup debounce
const int BUTTON_PIN = 0;
int buttonState = 0;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 150;    // the debounce time; increase if the output flickers

 
//setup of neopixels
#define PIXEL_PIN       16                      // Pin connected to the NeoPixel data input.
#define PIXEL_COUNT     21                      // Number of NeoPixels.
#define PIXEL_TYPE      NEO_GRB + NEO_KHZ800   // Type of the NeoPixels (see strandtest example).
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE); // create NeoPixels object



void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT);
  // Initialize NeoPixels 
  pixels.begin();
  
  Serial.begin(115200);
  
  // We start by connecting to a WiFi network
   WiFiManager MyWifiManager;
   MyWifiManager.autoConnect("Family_Lamp");
  Serial.println("Connected to the WiFi network");
  //connects to mqtt server
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

  //publishes client id and subscribes to colors 
  client.publish("Family_Lamps/Client_ID", CELINT_ID);
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
  buttonState = digitalRead(BUTTON_PIN);
    if ( (millis() - lastDebounceTime) > debounceDelay) 
    {
 
      //if the button has been pressed, lets toggle the LED from "off to on" or "on to off"
      if (buttonState == LOW) 
      {
        client.publish("Family_Lamps/Incoming_Colors", "button press");
        lastDebounceTime = millis(); //set the current time
      }

    }
}
