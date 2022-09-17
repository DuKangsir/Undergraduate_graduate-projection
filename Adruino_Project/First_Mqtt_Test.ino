#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Serendipity";
const char* password = "88888888";
const char* mqttServer = "iot.ranye-iot.net";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char charBuf[391];
const char* mqttUserName = "GreenHouse_IoT_1";         // 服务端连接用户名
const char* mqttPassword = "Dk200010";          // 服务端连接密码
const char* clientId = "GreenHouse_IoT_1_id";          // 客户端id 
const char* subTopic = "GreenHouse_IoT/Env_kz";        // 订阅主题
const char* pubTopic = "GreenHouse_IoT/Env_zt";        // 发布主题
const char* willTopic = "GreenHouse_IoT/Env_yz";       // 遗嘱主题名称

const char* willMsg = "esp8266 offline";        // 遗嘱主题信息
const int willQos = 0;                          // 遗嘱QoS
const int willRetain = false;                   // 遗嘱保留

const int subQoS = 1;            // 客户端订阅主题时使用的QoS级别
const bool cleanSession = false; // 清除会话


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  connectWifi();
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(receiveCallback);
  connectMQTTserver();    
}

void loop() {
if (!mqttClient.connected()) {
    connectMQTTserver();
  }
  mqttClient.loop();
}

void connectWifi(){
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");  
  Serial.println(""); 
}

void connectMQTTserver(){
  if (mqttClient.connect(clientId, mqttUserName, 
                         mqttPassword, willTopic, 
                         willQos, willRetain, willMsg, cleanSession)) { 
    Serial.print("MQTT Server Connected. ClientId: ");
    Serial.println(clientId);
    Serial.print("MQTT Server: ");
    Serial.println(mqttServer);    
    
    subscribeTopic(); // 订阅指定主题
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(5000);
  }   
}

void subscribeTopic(){
  if(mqttClient.subscribe(subTopic, subQoS)){
    Serial.print("Subscribed Topic: ");
    Serial.println(subTopic);
  } else {
    Serial.print("Subscribe Fail...");
  }  
}

void pubMQTTmsg(){ 
      if(Serial.available()>0){
      delay(100);
      String  comdata = Serial.readString();
      Serial.print("Serial.readString:");
      Serial.println(comdata);
      while(Serial.read()>= 0){}
 
      comdata.toCharArray(charBuf, 50) ;     
      mqttClient.publish(pubTopic,charBuf);
      comdata = "";
    } 
 }

void receiveCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  Serial.print("Message Length(Bytes) ");
  Serial.println(length);
  pubMQTTmsg();
}
