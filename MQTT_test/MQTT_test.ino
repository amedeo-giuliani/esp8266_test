#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <String.h>

const char* ssid = "...";
const char* password = "...";
const char* id = "test_light";
const char* topic = "esp8266/led";
const char* state_topic = "esp8266/led/state";

IPAddress broker(192,168,178,22);
WiFiClient espclient;

PubSubClient client(espclient);

void callback(char* topic, byte* payload, unsigned int length){
  String message;

  for(int i = 0; i < length; i++){
    message += (char)payload[i];
  }

  Serial.print("Got message from [ ");
  Serial.print(topic);
  Serial.print(" ] : ");
  Serial.println(message);

  //logica negata
  if(message == "on"){
    digitalWrite(2,LOW);
    client.publish(state_topic,"on");
  }
  else if(message == "off"){
    digitalWrite(2,HIGH);
    client.publish(state_topic,"off");
  }
}

void connectToWiFi(){
  WiFi.enableSTA(true);
  delay(100);
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Waiting WiFi...");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());  
}

void mqtt_reconnect(){
  while(!client.connected()){
    Serial.println("Reconnection");
    if(client.connect(id)){
      client.subscribe(topic);
      Serial.println("connected");
      Serial.print("Subscribed to: ");
      Serial.println(topic);
    }
    else{
      Serial.println("Trying again in 5 sec");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  delay(100);
  connectToWiFi();
  client.setServer(broker,1883);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()){
    mqtt_reconnect();
  }
  client.loop();
}
