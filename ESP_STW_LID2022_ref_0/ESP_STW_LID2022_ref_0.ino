/*

ESP Steering wheel example ref 1.2.2022 KN
Lidar reading
  
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>



            //            network credentials


const char* ssid = "SOURCE";
const char* password = "Pelle!23";

//const char* ssid = "mokkula_925936";
//const char* password = "12345678";


bool    spiffsActive = false;

                                                                        // Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String from_mega="";                                                    // lidar distance value in String format

int is;

char buf[40];                                                           //  Serial bus buffer   


 
void setup()
{
                                                                          // Serial port for transfer data with Mega
  Serial.begin(9600);
  
  Serial.println("");

  Serial.println("----------------");

  Serial.println("----------------");

                                                                          // Start file subsystem
  if (SPIFFS.begin()) {
      Serial.println("SPIFFS Active");
      Serial.println();
      spiffsActive = true;
  } else {
      Serial.println("Unable to activate SPIFFS");
  }


                                                                            // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

                                                                            // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  

                                                                            // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
                request->send(SPIFFS, "/index.html", String(), false, processor);

 //             Serial.println(" HTML GET ");

                int paramsNr = request->params();

 //             Serial.println(" ------ ");
 //             Serial.print("Param qty: ");
 //             Serial.println(paramsNr);
      
                for (byte i=0;i<paramsNr;i++)
                    {

                        AsyncWebParameter* p = request->getParam(0);
 
//                      Serial.print("Param name: ");
//                      Serial.println(p->name());

                        if ((p->name())=="dist"){ Serial.print("Move:");Serial.println(p->value()); };
                        if ((p->name())=="dire"){ Serial.print("Turn:");Serial.println(p->value()); };
      
   //                   Serial.print("Param value: ");
   //                   Serial.println(p->value());
                    };                                                          //    end of for loop
                    
   
              });                                                              // server on END



                                                                              // Route to load style.css file
  server.on("/jquery-1.11.3.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/jquery-1.11.3.min.js", "text/javascript");

    Serial.println(" JS jquery GET ");
      
  });

                                                                                // Route to load style.css file
  server.on("/roundslider.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/roundslider.min.js", "text/javascript");

    Serial.println(" min JS GET ");
      
  });

  
                                                                                  // Route to load style.css file
  server.on("/roundslider.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/roundslider.min.css", "text/css");
    
    Serial.println(" min css GET ");
    
  });


                                                                                //  Method GET - query from MEGA to to web page tag is / from_MEGA
  server.on("/from_MEGA", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      request->send_P(200, "text/plain", string_to_JS().c_str());               // return value to web page JS
      
    }); 


                                                                                  // Start server
  server.begin();

}                                                                                 //  END OF setup



                                                                                  // reservation 
String processor(const String& var)
{  }




String string_to_JS()                                                             // funtion to be called with GET from_MEGA tag
  {
    return from_mega;
  }


 
void loop()
{
  if (Serial.available() > 0) 
    {
      serial_read();                                            // read string from Mega
      from_mega=buf;
       
    }  
}





void serial_read()                                    // read serial in string, save received characters to buff[]
{
  boolean done=0; 

  while (done==0)                                     // wait end of message LF
    {
        while(Serial.available() > 0)
        {
          char char_in = Serial.read();
        
          if(char_in>13)                              // is input char above CR
                {   
                 buf[is]=char_in;
                 is++;
                }
  
          if(char_in==10){                            // check if end of message  LF   
                    buf[is]=0;                        // end of string
                    is=0;
                    done=1;
                  }
    
        }                                            // end of while serial available  

        if(done==1) { done=0; break; }

  }                                                 // while END , End of message  LF
   
      // Serial.println(buf);
      
}                                                   // end of serial read
