#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

#define WIFI_SSID "xxxxxxx"                    // <----- Set SSID !
#define WIFI_PASSWORD "xxxxxx"                 // <----- Set Password

#define MQTT_HOST IPAddress(1, 1, 1, 1)        // <----- Set Broker IP
#define MQTT_PORT 1883


#if ASYNC_TCP_SSL_ENABLED                      // <----- Enable or disable TLS
#define MQTT_SECURE true
#define MQTT_SERVER_FINGERPRINT {}             // <<---- set certificate fingerprint
#define MQTT_PORT 1883
#else
#define MQTT_PORT 1883
#endif







AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long WiFiConnectionLoop = 2000;
unsigned long maxSleepingTime = 10000;       // <--- Variable T
unsigned long startSetup = 0;
unsigned long Puffer = 500;
unsigned long counter_R = 0;                
unsigned long R = 0;                 //  <--- Variable R

unsigned long publishProcessStarted = 0;
unsigned long waitForPacketId = 0;
unsigned long publishSucess = 0;
unsigned long publishQoS = 0;





void connectToWifi() {
  Serial.println("Try Connecting to WiFi ...");
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startLoop = millis();
  while( ( WiFiConnectionLoop > millis()-startLoop)  &&  (WiFi.status() != WL_CONNECTED) ){
    delay(200);
    Serial.print(".");
  }

}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  if((WiFi.status() == WL_CONNECTED)){
    mqttClient.connect();
  }
  else{
    connectToWifi();
  }
}


void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}



void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);                                   
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {}

void onMqttUnsubscribe(uint16_t packetId) {}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {}

void onMqttPublish(uint16_t packetId) {
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  startSetup = millis();
  publishProcessStarted = 0;
  waitForPacketId = 0;
  publishSucess = 0;

  Serial.println("Start Set Up");

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  #if ASYNC_TCP_SSL_ENABLED
  mqttClient.setSecure(MQTT_SECURE);
  if (MQTT_SECURE) {
    mqttClient.addServerFingerprint((const uint8_t[])MQTT_SERVER_FINGERPRINT);
  }
  #endif

  connectToWifi();
}



void loop() {
    
    if(counter_R < R){
      if( ( millis() - startSetup ) > ( maxSleepingTime ) ){
      publishProcessStarted = 0;
      startSetup = millis();   
    }
    else{
      if(publishProcessStarted==0){
        if(mqttClient.connected()){                                       
            waitForPacketId = mqttClient.publish("topic", publishQoS, true, "xxxxx");           // <--- Variable B:    Replace xxxx with the payload, Variable t_0: Replace topic with topic
            publishProcessStarted = 1;
        }
      }
    }

    }
    else{
      ESP.deepSleep(300e6);
    }

}

