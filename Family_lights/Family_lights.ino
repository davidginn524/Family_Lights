#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
Ticker ticker;
WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "#############################";
const int port = ****;

const int red = 5;
const int green = 4;
const int blue = 12;

const int led = 16;
const int touch = 14;
const char* lampColour = "010";
const char* lampName = "LampGreen";
int currentColour[3] = {0,1,0};
int newColour[3] = {0,1,0};

long sampleTime = 0;         // the last time the output pin was sampled
int debounce_count = 100; // number of millis/samples to consider before declaring a debounced input
int counter = 0;
int reading;           // the current value read from the input pin
int current_state = LOW;    // the debounced input value

unsigned long turnOffMillis = 0;
int flag = 0;

int duty = 0;
const int steps = 64;
const int fadeSpeed = 50;
const int lookup[64] = {0,0,0,1,1,1,1,2,2,3,3,
4,4,5,6,7,8,9,10,11,12,13,
14,15,16,18,19,21,22,24,25,27,28,
30,32,34,36,38,40,42,44,46,48,50,
52,55,57,60,62,65,67,70,72,75,78,
81,84,87,90,93,96,99,99,99};

void setColour(int[3]);

void tick()
{
  //toggle state
  int state = digitalRead(led);  // get the current state of GPIO1 pin
  digitalWrite(led, !state);     // set pin to the opposite state
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] -");
  Serial.print(length);  
  Serial.print("- ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    newColour[i]=((int)payload[i] - 48);
  }  
  Serial.println();
  Serial.println(newColour[1]);
  setColour(newColour);
  turnOffMillis = millis();
  flag = 1;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(lampName,"","")) {
      Serial.println("Connected to the MQTT Server");
      // Once connected, publish an announcement...
      client.publish("online", lampName);
      // ... and resubscribe
      client.subscribe("colourChange");
      digitalWrite(led, LOW);
    } else {
      digitalWrite(led, HIGH);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  ticker.detach();
  ticker.attach(0.3, tick);
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void setup() {
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(touch, INPUT);
  digitalWrite(led, 1);
  
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.7, tick);
  
  Serial.begin(115200);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConnectTimeout(20);

  //reset saved settings
  //wifiManager.resetSettings();
    
  wifiManager.autoConnect("AutoConnectAP");
  
  //if you get here you have connected to the WiFi
  Serial.println("Connected to WiFi:)");
  
  ticker.detach();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  digitalWrite(led, LOW);
    
  setColour(currentColour);
  flag = 1;
  turnOffMillis = millis(); 
}

void loop()
{      
  if ( flag == 1 && (millis() - turnOffMillis) > 3600000 )
  {
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      flag = 0;
  }

  if ( millis() != sampleTime)
  {
    reading = digitalRead(touch);
    if(reading == current_state && counter > 0)
    {
      counter--;
    }
    if(reading != current_state)
    {
       counter++; 
    }
    // If the Input has shown the same value for long enough let's switch it
    if(counter >= debounce_count)
    {
      counter = 0;
      current_state = reading;
      Serial.println(current_state);
      if (current_state == 1) {
        client.publish("colourChange",lampColour);
      }
    }
    sampleTime = millis();
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


void setColour(int newColour[3]) 
{
  for (int i=(steps - 1); i>=0; i--)
  {
    duty = lookup[i];
    for (int j=0; j<fadeSpeed; j++)
    {
      // one pulse of PWM
      digitalWrite(red, (int)currentColour[0]);
      digitalWrite(green, (int)currentColour[1]);
      digitalWrite(blue, (int)currentColour[2]);
      delayMicroseconds(duty);
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      delayMicroseconds(100-duty);
    }
  }

  for (int i=1; i<steps; i++)
  {
    duty = lookup[i];
    for (int j=0; j<fadeSpeed; j++)
    {
      // one pulse of PWM
      digitalWrite(red, (int)newColour[0]);
      digitalWrite(green, (int)newColour[1]);
      digitalWrite(blue, (int)newColour[2]);
      delayMicroseconds(duty);
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(blue, LOW);
      delayMicroseconds(100-duty);
    }
  }
  digitalWrite(red, (int)newColour[0]);
  digitalWrite(green, (int)newColour[1]);
  digitalWrite(blue, (int)newColour[2]);
  currentColour[0]=newColour[0];
  currentColour[1]=newColour[1];
  currentColour[2]=newColour[2];
}
