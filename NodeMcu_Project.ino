

/*      头文件*/
#include "DHTesp.h"
#include <Arduino.h>
#include <U8x8lib.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/*     引脚号宏定义*/
#define LED D5
#define BEEP D7
#define DHT_11 0
#define OLED_SCL D1
#define OLED_SDA D2


/*   全局变量（对象）*/


DHTesp dht;
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/OLED_SCL, /* data=*/OLED_SDA, /* reset=*/ U8X8_PIN_NONE);
unsigned long previousMillis = 0;
const long interval = 2000;
int Pin_state = LOW; 

const char* ssid = "";//wifi名称
const char* password = "";
const char* mqtt_server = "";//mqtt服务器
const char* pubTopic = "/NodeMcu/pub";//上行topic
const char* subTopic = "/NodeMcu/sub";//下行topic


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (64)
char msg[MSG_BUFFER_SIZE];


//wifi设置函数
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  String cmd = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    cmd += (char)payload[i]; 
    Serial.print((char)payload[i]);
  }
  Serial.println(cmd);

  const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& root = jsonBuffer.parseObject(cmd);
  //root.containskey("city");
  if(root.containsKey("led")){
     int led = root["led"];
     if(led){
      digitalWrite(LED, HIGH);//led灯开
      }else{
        digitalWrite(LED, LOW);
        }
    }
    if(root.containsKey("beep")){
     int beep = root["beep"];
     if(beep){
      digitalWrite(BEEP, HIGH);//蜂鸣器开
      }else{
        digitalWrite(BEEP, LOW);
        }
    }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(pubTopic, "hello world");
      // ... and resubscribe
      client.subscribe(subTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/*   初始化函数*/
void setup() {

  Serial.begin(115200);
  
  
  //Led初始化
  pinMode(LED, OUTPUT); 
  pinMode(BEEP, OUTPUT);

  digitalWrite(BEEP, HIGH);
  digitalWrite(LED, HIGH);
  
  //DHT11初始化
  
  dht.setup(DHT_11, DHTesp::DHT11 );

  //初始化OLED
  u8x8.begin();
  u8x8.setFont(u8x8_font_8x13_1x2_r);
  u8x8.setPowerSave(0);
  u8x8.drawString(0,0,"Hello,Leo");



  //wifi
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
}





/*     主循环函数*/
void loop() {
   

 unsigned long currentMillis = millis();
 float humidity = dht.getHumidity();
 float temperature = dht.getTemperature();


   //wifi
 if (!client.connected()) {
    reconnect();
  }
  client.loop();

  
  //Led
 if(currentMillis - previousMillis >= interval ){
    previousMillis = currentMillis;
    char displaybuf[32];
    memset(displaybuf,0,sizeof(displaybuf));
    
//    if (Pin_state == LOW){
//       Pin_state = HIGH;
//      }else{
//        Pin_state = LOW;
//        }
//  
////     digitalWrite(LED, Pin_state);
////     digitalWrite(BEEP, Pin_state);   
//  
 

  //DHT11获取温湿度数据
    
  if(dht.getStatusString() == "OK"){

     
     Serial.print("\t");
     Serial.print(humidity, 1);
     Serial.print("\t\t");
     Serial.print(temperature, 1);
     Serial.print("\t\t");
     
    
    sprintf(displaybuf,"Temp: %.1f C ",temperature);
    u8x8.drawString(0,2,displaybuf);
    sprintf(displaybuf,"Hum: %.1f %% ",humidity);
    u8x8.drawString(0,4,displaybuf);

    }
    
    snprintf (msg, MSG_BUFFER_SIZE,"{\"Temp\":%.1f,\"Hum\":%.1f,\"Led\":%d,\"Beep\":%d}", temperature,humidity,digitalRead(LED),digitalRead(BEEP));
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(pubTopic, msg);
    

  
 }


    
}
