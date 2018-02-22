#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

//libraries for auto wifi setup
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>


//mqtt server variables test variables to be decom later
const char* mqttServer = "m23.cloudmqtt.com";
const int mqttPort = 16743;
const char* mqttUser = "crjhzcfb";
const char* mqttPassword = "OgX58L0IebEa";
 
WiFiClient espClient;
PubSubClient client(espClient);
//cleint ID for hashing and debuging set both to the same 
const char* CLIENT_ID = "1";
char CLIENT_ID_INT = '1';

  
//define button pin and setup debounce
int BUTTON_PIN = 0;
int buttonState = 0;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 150;    // the debounce time; increase if the output flickers

 
//setup of neopixels
#define PIXEL_PIN       12                      // Pin connected to the NeoPixel data input.
#define PIXEL_COUNT     16                    // Number of NeoPixels.
#define PIXEL_TYPE      NEO_GRB + NEO_KHZ800   // Type of the NeoPixels (see strandtest example).
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE); // create NeoPixels object
//setup int for checking if LEDs have been lit previously
unsigned int previouslylit;

void setup() {
  pixels.begin();
  pixels.show();

  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT);
  // Initialize NeoPixels 
 
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
 
    if (client.connect(CLIENT_ID, mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }

  //publishes client id and subscribes to colors 
  client.publish("Family_Lamps/Client_ID", CLIENT_ID);
  client.subscribe("Family_Lamps/Incoming_Colors");


}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
   Serial.println(" ");
   //Serial.print("Payload Array 0 = "); 
   // Serial.println((char)payload[0]);
   // Serial.println(CLIENT_ID_INT);
   //i published the hash 
    if (payload[0] == CLIENT_ID_INT )
    {Serial.println("I published the hash");
     Serial.println(payload[0]);
     //Since I was the one that published the hash do rainbow cycle for a few sec then change to the generated random color 
    }
    else
    //someone else published the hash 
    {
      Serial.println(previouslylit);
      //if previously lit fade out current color then fade in new color 
      if (previouslylit == 1) 
      {
         for (int light = 255; light>=0; --light)
        {
          for (int i=0; i<PIXEL_COUNT; ++i) 
          {
            pixels.setPixelColor(i, payload[2], payload[6], payload[8]);
           // Serial.println (i);
          }   
          pixels.setBrightness(light);
          Serial.println (light);
          pixels.show();
          delay(20);
        }
           previouslylit = 1;
          Serial.println("Previously lit");
          Serial.println("Someone else published the hash");
      }
      //if not previously lit fade in new color 
      else
      {
        for (int light = 0; light<=255; ++light)
        {
          for (int i=0; i<PIXEL_COUNT; ++i) 
          {
            pixels.setPixelColor(i, payload[2], payload[6], payload[8]);
           // Serial.println (i);
          }   
          pixels.setBrightness(light);
          //Serial.println (light);
          pixels.show();
          delay(20);
        }
          previouslylit = 1;
          Serial.println(previouslylit);
          Serial.println("not lit");
          Serial.println("Someone else published the hash");
      }
    }
  Serial.println();
  Serial.println("-----------------------");
}
 
void loop() {
  client.loop();
  String CID= String(CLIENT_ID);
  String randomred = String(random(255));
  String randomgreen  = String(random(255));
  String randomblue = String(random(255));
  String Color_Value = String(CID+"G"+ randomgreen +"R"+randomred +"B"+randomblue);
  char Client_Hash[30];
  Color_Value.toCharArray(Client_Hash, 30);
   
  buttonState = digitalRead(BUTTON_PIN);
    if ( (millis() - lastDebounceTime) > debounceDelay) 
    {
      
      //if the button has been pressed, lets toggle the LED from "off to on" or "on to off"
      if (buttonState == LOW) 
      {
        client.publish("Family_Lamps/Incoming_Colors",Client_Hash );
 
        lastDebounceTime = millis(); //set the current time
      }

    }
}


