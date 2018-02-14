#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(10,0,0,5);
const char* server = “m23.cloudmqtt.com”;
const int mqttPort = 17393;
const char* mqttUser = “----”;
const char* mqttPassword = “----”;

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print(“Message arrived [”);
Serial.print(topic);
Serial.print("] ");
for (int i=0;i<length;i++) {
Serial.print((char)payload[i]);
}
Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
// Loop until we’re reconnected
while (!client.connected()) {
Serial.print(“Attempting MQTT connection…”);
// Attempt to connect
if (client.connect("")) {
Serial.println(“connected”);
// Once connected, publish an announcement…
client.publish(“outTopic”,“hello”);
// … and resubscribe
client.subscribe(“inTopic”);
} else {
Serial.print(“failed, rc=”);
Serial.print(client.state());
Serial.println(" try again in 5 seconds");
// Wait 5 seconds before retrying
delay(5000);
}
}
}

void setup()
{
Serial.begin(57600);

client.setServer(server, 19405);
client.setCallback(callback);

Ethernet.begin(mac, ip);
// Allow the hardware to sort itself out
delay(1500);
}

void loop()
{
if (!client.connected()) {
reconnect();
}
client.loop();
}
