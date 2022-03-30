
//////////////////////////////////////////////////
///////// MQTT ESP8266 TEST 2022 03 21 
//////////////////////////////////////////////////


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"

#define led1 4

#define PWMRANGE 255

// Update these with values suitable for your VM.
//#define MQTT_ID  "ESP8266_2022_31"
#define MQTT_ID  "Fun_ESP"  //random this id
const char* mqtt_server = "172.16.4.194";
#define intopic  "esp_in"
#define outtopic  "esp_out"


void connect(void);
int print_AD(void);
void setup_wifi(void);
void callback(char* topic, byte* payload, unsigned int length);



// Update these with values suitable for your network.
const char* ssid = "SOURCE";
const char* password = "Pelle!23";


WiFiClient espClient;
PubSubClient client(espClient);

DynamicJsonDocument doc(100);

////////////////////////////////////////////////////////////////////// setup start 
void setup() {

  Serial.begin(115200);

  pinMode(led1, OUTPUT);  // Initialize pin
  digitalWrite(led1, 1);

  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(0, OUTPUT);


  Serial.println("START 2022");

  delay(1000);
    
  
  setup_wifi();                               // init Wifi
  client.setServer(mqtt_server, 1883);        // set server IP and port number
  client.setCallback(callback);               // define receive callback
  
  connect();                                  // connect to mqtt server
  
  
}
///////////////////////////////////////////////////////////////////////   setup end

///////////////////////////////////////////////////////////////////////   loop start

void loop() 
{

  if (!client.connected()) {  connect();  }  // reconnect if needed

  while(1)

    {
      client.loop();
    
      int sum=analogRead(A0); 
      digitalWrite(5, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(0, HIGH);

      Serial.print(" AD converter= ");  Serial.println(sum);
  
      char msg1[50]="{\"TFL\":1}";
      char msg2[50]="{\"TFL\":2}";
      char msg3[50]="{\"TFL\":3}";
      char msg4[50]="{\"TFL\":4}";
           
      Serial.print("Publish message: "); 
      
      if (sum < 300){
        Serial.println(msg1);          
        //send/publish
        client.publish(outtopic, msg1);
      } else if (sum >= 300 && sum < 600 ){
        Serial.println(msg2);          
        //send/publish
        client.publish(outtopic, msg2);
      } else if (sum >= 600){
        Serial.println(msg3);          
        //send/publish
        client.publish(outtopic, msg3);
      }
      
           
      delay(1000);
      
    }  ////         while end
}  /////////////////////////////////////////////////////////////         LOOP END 

////////////////////////////////////////////////////////////////         setup WIFI
void setup_wifi() 
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());                       ////////   Print IP number
}///////////////////////////////////////////////////////////////   setup WIFI end


/////////////////////////////////////////////////////////////// callback when message arrive
void callback(char* topic, byte* payload, unsigned int length) 
{
   
  Serial.print("Message arrived :");
  char tp[30];
  snprintf(tp,length,"%s", payload);
  Serial.println(tp);
  

   parse_msg(payload);  

  
}


///////////////////////////////////////////////////////////////////  connect to MQTT broker  
void connect() {        
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_ID)) {
      Serial.println("connected to MQTT broker");
      client.subscribe(intopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




void parse_msg(uint8_t *data)                                               // parse message
{
    Serial.println("Parsing start: ");
    
    DeserializationError error = deserializeJson(doc, data);                // parse json message
                                              
    if (error) {                                                            // Test if parsing succeeds.
                  Serial.print(F("deserializeJson() failed: "));
                  Serial.println(error.f_str());
                  return;
                }
                
    JsonObject root = doc.as<JsonObject>();                                 // 

    String com;                                                             // (JSON) name ( =command)
    double val;                                                             // (JSON) value

    for (JsonPair kv : root) 
      {
             com=(kv.key().c_str());
             val=kv.value().as<double>();
       }
       
    Serial.print(" (Json pair/name):");Serial.print(com);Serial.print(" (json pair/value):");Serial.println(val);
                               

} // end parse message
