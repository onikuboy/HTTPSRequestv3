 /*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */  
 
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>


//URL for Console Setup Information
//https://script.google.com/macros/s/AKfycbxvHBxaTbdzd-aZwbpdBqqsOLM2rcmcMc7OLCnTavT7xTcqFNmF/exec

//URL for Location Log
//https://script.google.com/macros/s/AKfycbwAxDFvGIvkDAUfSuE_Wm2MKxMpZKqwaqG7HZGD2wZ8X3GMDzw/exec

//#define DEBUG 1

const char* host = "script.google.com";
const int httpsPort = 443;
const char* Googlehost = "google.com.sg";
const int httpPort = 80;
char Stages;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";
const String url_Setup = "/macros/s/AKfycbxvHBxaTbdzd-aZwbpdBqqsOLM2rcmcMc7OLCnTavT7xTcqFNmF/exec?";  
const String url_Log = "/macros/s/AKfycbwAxDFvGIvkDAUfSuE_Wm2MKxMpZKqwaqG7HZGD2wZ8X3GMDzw/exec?";
                                   
const String url_Console = "Console=";
const String url_Postal = "&Postal=";
const String url_Unit = "&Unit=";
const String url_Day = "&Day=";
const String url_Night = "&Night=";
const String url_Emergency = "&Emergency=";
const String url_Server = "&Server=";
const String url_Main = "&Main=";
const String url_IP = "&IP=";
const String url_Period = "&Period=";
const String url_LightOut = "&LightOut=";
const String url_Date = "&Date=";
const String url_Time = "&Time=";
const String url_Location = "&Location=";
const String Username_line = "admin";
const String Password_line = "admin";

char httpReply[4096];
uint32_t TimeOut_Counter;

char Router[80];
char PassWord[80];
char GuardianID[10];
char GuardianPostal[10];
char GuardianUnit[10];
char GuardianDay[10];
char GuardianNight[10];
char GuardianEmergency[20];
char GuardianServer[20];
char GuardianMain[20];
char GuardianIP[20];
char CommandBuffer[128];
char GuardianPeriod[10];
char GuardianLightOut[10];
char Log[512][11];
char Time[4];
char Date[6];
char Server_SSID[9];

String Router_line;
String Pwd_line;
String URL_line;
String line;
String Command;

int count;
int loop_count,index_count;
String Console_line;
String Postal_line;
String Unit_line;
String Day_line;
String Night_line;
String Emergency_line;
String Server_line;
String Main_line;
String IP_line;
String Period_line;
String LightOut_line;
String AP_line;
String AP_pwd;

enum OPERATION_STAGE
{
  Command_Idle, Command_Pending, Setup_Command, Log_Command, Start_Command, Time_Command, Time_Received, Setup_Sent, Command_Received, Start_Log, Test_Date
};

ESP8266WebServer server;
//WiFiServer server(80);

WiFiClientSecure httpsclient;
WiFiClient GoogleClient;


void setup() {
  Serial.begin(115200);
  Serial.println();
  Stages = Command_Idle;


  TimeOut_Counter = 100000;
 
  WiFi.disconnect();

#ifdef DEBUG  
  Serial.print("WiFi mode is at ");
  Serial.println(WiFi.getMode());
#endif

  delay(500);
}

void loop() {


  switch(Stages)
  {
    case Command_Idle : 
                          server.handleClient();  
                          line = "";
                          if(Serial.available() > 0)
                          {
                            Serial.readBytesUntil('\n',&CommandBuffer[0],128);
                            Stages = Command_Received;
                          }
                        
                        break;

    case Command_Received :   count = 0;
                              while(CommandBuffer[count] != '\r')
                              {
                                line += CommandBuffer[count];
                                count++; 
                              }
#ifdef DEBUG
                              Serial.println(line);
#endif                  
                              Command = "";
                              
                              for(loop_count = 0 ; loop_count < 6 ; loop_count++)
                              {
                                Command += CommandBuffer[loop_count];    
                              }
                              
                              if(Command == "Setups")
                              {
                                URL_line = "";
                                URL_line = url_Console;
                                loop_count = 7;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianID[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                                                     
                                }


                                URL_line += url_Postal;
                                loop_count++;
                                count = 0;

                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianPostal[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }

                                if(GuardianPostal[0] != '?')
                                {
#ifdef DEBUG
                                  Serial.println(URL_line);
#endif                                                                

                                  Server_SSID[0] = 'A';
                                  Server_SSID[1] = 'n';
                                  Server_SSID[2] = 'g';
                                  Server_SSID[3] = 'e';
                                  Server_SSID[4] = 'l';
                                  Server_SSID[5] = GuardianID[0];
                                  Server_SSID[6] = GuardianID[1];
                                  Server_SSID[7] = GuardianID[2];
                                  Server_SSID[8] = GuardianID[3];

                                  WiFi.softAP(Server_SSID,Server_SSID);
#ifdef DEBUG
                                  Serial.print("WiFi connection is ");
                                  Serial.println(WiFi.isConnected());
#endif
                                  WiFi.reconnect();
                                  while (WiFi.status() != WL_CONNECTED)
                                  {
                                    delay(500);
#ifdef DEBUG
                                    Serial.print(".");
#endif
                                  } 
                                                                
#ifdef DEBUG
                                  Serial.print("WiFi mode is at ");
                                  Serial.println(WiFi.getMode());
#endif                                
                                }

#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                

                                URL_line += url_Unit;
                                loop_count++;
                                count = 0;

                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianUnit[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }

#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                

                                URL_line += url_Day;
                                loop_count++;
                                count = 0;

                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianDay[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }

#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                
                                URL_line += url_Night;
                                loop_count++;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianNight[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }

#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                
                                URL_line += url_Emergency;
                                loop_count++;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianEmergency[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }

#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                
                                URL_line += url_Server;
                                loop_count++;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianServer[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }
 
#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                
                                URL_line += url_Main;
                                loop_count++;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianEmergency[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }

#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                
                                URL_line += url_IP;
                                loop_count++;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianIP[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }
                                
#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                
                                URL_line += url_Period;
                                loop_count++;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != ',')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianPeriod[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }


#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                
                                URL_line += url_LightOut;
                                loop_count++;
                                count = 0;
                                
                                while(CommandBuffer[loop_count] != '\r')
                                {
                                  URL_line += CommandBuffer[loop_count];
                                  GuardianLightOut[count] = CommandBuffer[loop_count];
                                  loop_count++;
                                  count++;
                                                                    
                                }

#ifdef DEBUG
                                Serial.println(URL_line);
#endif                                                                

                                Stages = Setup_Command;
                              }
                              else
                              {
                                if(Command == "Logger")
                                {

                                  URL_line = "";
                                  URL_line = url_Console;
                                  loop_count = 7;
                                  while(CommandBuffer[loop_count] != ',')
                                  {
                                    URL_line += CommandBuffer[loop_count];
                                    loop_count++;
                                  }

                                  URL_line += url_Date;
                                  loop_count++;

                                  while(CommandBuffer[loop_count] != ',')
                                  {
                                    URL_line += CommandBuffer[loop_count];
                                    loop_count++;
                                  }

                                  URL_line += url_Time;
                                  loop_count++;

                                  while(CommandBuffer[loop_count] != ',')
                                  {
                                    URL_line += CommandBuffer[loop_count];
                                    loop_count++;
                                  }

                                  URL_line += url_Location;
                                  loop_count++;
                                
                                  while(CommandBuffer[loop_count] != '\r')
                                  {
                                    URL_line += CommandBuffer[loop_count];
                                    loop_count++;
                                  }

                                  
                                  Stages = Start_Log;
                                }
                                else
                                {
                                  if(Command == "Starts")
                                  {
                                    loop_count = 7;
                                    Router_line = "";
                                    while(CommandBuffer[loop_count] != ',')
                                    {
                                      Router_line += CommandBuffer[loop_count];
                                      loop_count++;
                                    
                                    }

#ifdef DEBUG
                                    Serial.println(Router_line);
#endif                                    
                                    loop_count++;
                                    Pwd_line = "";

                                    while(CommandBuffer[loop_count] != '\r')
                                    {
                                      Pwd_line += CommandBuffer[loop_count];
                                      loop_count++;
                                    
                                    }
#ifdef DEBUG
                                    Serial.println(Pwd_line);
#endif                                    
                                    
                                    Stages = Start_Command;
                                  }
                                  else
                                  {
                                    if(Command == "Timers")
                                    {
                                      Stages = Time_Command;
                                    }
                                    else
                                    {
                                      if(Command == "Tester")
                                      {
                                        count = 0;;
                                        loop_count = 7;
                                        while(CommandBuffer[loop_count] != '\r')
                                        {
                                          httpReply[count] = CommandBuffer[loop_count];
#ifdef DEBUG
                                          Serial.print(httpReply[count]);
#endif                                                                                    
                                          loop_count++;
                                          count++;
                                        }                                        

                                        Stages = Test_Date;
                                      }
                                      else
                                      {
#ifdef DEBUG                        
                                        Serial.print("Invalid Inputs ");
#endif
                                        Stages = Command_Idle;
                                      }
                                    }
                                  }
                                }
                              }
                            
                            break;
   
    case Start_Command  : 

#ifdef DEBUG      
                            Serial.print("connecting to ");
                            Serial.println(Router_line);
#endif
                            WiFi.begin(Router_line.c_str(),Pwd_line.c_str());
                  

                            while (WiFi.status() != WL_CONNECTED)
                            {
                              delay(500);
#ifdef DEBUG
                              Serial.print(".");
#endif
                            }


                            Serial.println("\r\nWiFi connected");
                            Serial.println("IP address: ");
                            line = "";
                            line = WiFi.localIP().toString();
                            count = sizeof(line);
                            
                            while(count < 15)
                            {
                              line += ' ';
                              count++;  
                            }
                            Serial.println(line);

                            server.on ( "/", handleRoot );
                            server.on ( "/login", handleLogin );
                            server.on ( "/log", handleLog );
                            server.on ( "/test.svg", drawGraph );
                            server.on("/inline", [](){
                            server.send(200, "text/plain", "this works as well");
                            });
                            server.onNotFound ( handleNotFound );
                            server.begin();   

                            Stages = Command_Idle;
                          
                          
                          break;

    case Time_Command  : 
                            // Use WiFiClientSecure class to create TLS connection

#ifdef DEBUG    
                            Serial.print("connecting to ");
                            Serial.println(Googlehost);
#endif
                            delay(500);
                            
                            if (!GoogleClient.connect(Googlehost, httpPort)) 
                            {
                              Serial.println("connection failed");
                            }
                            else
                            {
                              String url = "/";
                              GoogleClient.print(String("HEAD ") + url + " HTTP/1.1\r\n" +
                                                 "Host: " + Googlehost + "\r\n" + 
                                                  "Connection: close\r\n\r\n");
                                                  
                              unsigned long timeout = millis();
    
                              while (GoogleClient.available() == 0) 
                              {
                                if (millis() - timeout > 5000) 
                                {
                                  Serial.println(">>> Client Timeout !");
                                  GoogleClient.stop();
                                  return;
                                }
                              }
                              
                              Stages = Time_Received;
                            }
                            break;

    case Time_Received  :   // Read all the lines of the reply from server and print them to Serial
                            count = 0;
                            //Read and parse all the lines of the reply from server          
                            while (GoogleClient.connected()) 
                            {
                              httpReply[count] = GoogleClient.read();
#ifdef DEBUG                            
                              Serial.print(httpReply[count]);
#endif
                              if(count == 2048)
                              {
                                count = 0;
                              }
                              else
                              {
                                count++; 
                              }
                    
                            }    

    case Test_Date      :   
#ifdef DEBUG  
                            Serial.println();
                            Serial.println("Client Connected");
#endif
                            for(count = 0 ; count < 2048 ; count++)
                            {
                    
                              if(httpReply[count] == 'D' && httpReply[count+1] == 'a' && httpReply[count+2] == 't' && httpReply[count+3] == 'e')
                              {
#ifdef DEBUG                              
                                Serial.println("Date Detected");
#endif                          
                                count += 11;

#ifdef DEBUG                              
                                
                                line = "";
                                
                                for(loop_count = 0 ; loop_count < 11 ; loop_count++)
                                {
                                  line += httpReply[count+loop_count];
                                }
            
                                Serial.println();
                                Serial.print("Date is ");
                                Serial.println(line);
#endif                          
                                Date[0] = httpReply[count];
                                Date[1] = httpReply[count+1];

                                line = httpReply[count+3];
                                line += httpReply[count+4];
                                line += httpReply[count+5];

                                if(line == "Jan")
                                {
                                  Date[2] = '0';
                                  Date[3] = '1';
                                }  
                                                
                                if(line == "Feb")
                                {
                                  Date[2] = '0';
                                  Date[3] = '2';
                                }
                                
                                if(line == "Mar")
                                {
                                  Date[2] = '0';
                                  Date[3] = '3';
                                }
                                
                                if(line == "Apr")
                                {
                                  Date[2] = '0';
                                  Date[3] = '4';
                                }
                                
                                if(line == "May")
                                {
                                  Date[2] = '0';
                                  Date[3] = '5';
                                }
                                
                                if(line == "Jun")
                                {
                                  Date[2] = '0';
                                  Date[3] = '6';
                                }
                                
                                if(line == "Jul")
                                {
                                  Date[2] = '0';
                                  Date[3] = '7';
                                }
                                
                                if(line == "Aug")
                                {
                                  Date[2] = '0';
                                  Date[3] = '8';
                                }
                                
                                if(line == "Sep")
                                {
                                  Date[2] = '0';
                                  Date[3] = '9';
                                }
                                
                                if(line == "Oct")
                                {
                                  Date[2] = '1';
                                  Date[3] = '0';
                                }
                                
                                if(line == "Nov")
                                {
                                  Date[2] = '1';
                                  Date[3] = '1';
                                }
                                
                                if(line == "Dec")
                                {
                                  Date[2] = '1';
                                  Date[3] = '2';
                                }
                                                
                                Date[4] = httpReply[count+9];
                                Date[5] = httpReply[count+10];
                                
                                count += 12;
#ifdef DEBUG                                 
                                                                
                                line = "";
                                
                                for(loop_count = 0 ; loop_count < 5 ; loop_count++)
                                {
                                  line += httpReply[count+loop_count];
                                }
                                                                
                             
                                Serial.println();
                                Serial.print("Time is ");
                                Serial.println(line);
#endif                          
                                Time[0] = httpReply[count];
                                Time[1] = httpReply[count+1];
                                Time[2] = httpReply[count+3];
                                Time[3] = httpReply[count+4];

                                int digit_hour = 0;
                                int digit_day = 0;
                                int digit_month = 0;
                                int digit_year = 0;
                                
                                digit_hour = Time[0];
                                digit_hour &= 0x0F;
                                digit_hour *= 10;
                                digit_hour += (Time[1] & 0x0F);
                                digit_hour += 8;

                                if(digit_hour >= 24)
                                {
                                  digit_hour -= 24;

                                  digit_day = Date[0];
                                  digit_day &= 0x0F;
                                  digit_day *= 10;
                                  digit_day += (Date[1] & 0x0F);

                                  digit_month = Date[2];
                                  digit_month &= 0x0F;
                                  digit_month *= 10;
                                  digit_month += (Date[3] & 0x0F);

                                  digit_year = Date[4];
                                  digit_year &= 0x0F;
                                  digit_year *= 10;
                                  digit_year += (Date[5] & 0x0F);

#ifdef DEBUG
                                  Serial.println();
                                  Serial.print("digit day is ");
                                  Serial.print(digit_day,DEC);
                                  Serial.println();

                                  Serial.println();
                                  Serial.print("digit month is ");
                                  Serial.print(digit_month,DEC);
                                  Serial.println();
                                  
                                  Serial.println();
                                  Serial.print("digit year is ");
                                  Serial.print(digit_year,DEC);
                                  Serial.println();

#endif                                  

                                  if(digit_month == 1 || digit_month == 3 || digit_month == 5 || digit_month == 7 || digit_month == 8 || digit_month == 10 || digit_month == 12)
                                  {
                                    if(digit_day < 31)
                                    {
                                      digit_day += 1;
                                    }
                                    else
                                    {
                                      if(digit_month < 12)
                                      {
                                        digit_day = 1;
                                        digit_month += 1;
                                      }
                                      else
                                      {
                                        digit_day = 1;
                                        digit_month = 1;
                                        digit_year += 1;
                                      }  
                                    }
                                  }
                                  else
                                  {
                                    if(digit_month == 4 || digit_month == 6 || digit_month == 9 || digit_month == 11)
                                    {
                                      if(digit_day < 30)
                                      {
                                        digit_day += 1;
                                      }
                                      else
                                      {
                                        digit_day = 1;
                                        digit_month += 1;
                                      }
                                    }
                                    else
                                    {
                                      if(digit_month == 2)
                                      {
                                    
                                        if((digit_year % 4) == 0)
                                        {
                                          if(digit_day >= 29)
                                          {
                                            digit_day = 1;
                                            digit_month += 1;       
                                          }
                                          else
                                          {
                                            digit_day += 1;
                                          }
                                        }
                                        else
                                        {
                                          if(digit_day >= 28)
                                          {
                                            digit_day = 1;
                                            digit_month += 1;
                                          }
                                          else
                                          {
                                            digit_day += 1;
                                          }
                                        }
                                      }
                                    }
                                  }

                                  if(digit_day < 10)
                                  {
                                    Date[0] = '0';
                                    Date[1] = (digit_day | 0x30);
                                  }
                                  else
                                  {
                                    Date[0] = (digit_day/10);
                                    Date[0] |= 0x30;
                                    Date[1] = (digit_day % 10);
                                    Date[1] |= 0x30;
                                  }

                                  if(digit_month < 10)
                                  {
                                    Date[2] = '0';
                                    Date[3] = (digit_month | 0x30);
                                  }
                                  else
                                  {
                                    Date[2] = (digit_month/10);
                                    Date[2] |= 0x30;
                                    Date[3] = (digit_month % 10);
                                    Date[3] |= 0x30;
                                  }

                                  if(digit_year < 10)
                                  {
                                    Date[4] = '0';
                                    Date[5] = (digit_year | 0x30);
                                  }
                                  else
                                  {
                                    Date[4] = (digit_year/10);
                                    Date[4] |= 0x30;
                                    Date[5] = (digit_year % 10);
                                    Date[5] |= 0x30;
                                  }

                                  if(digit_hour < 10)
                                  {
                                    Time[0] = '0';
                                    Time[1] = (digit_hour | 0x30);
                                  }
                                  else
                                  {
                                    Time[0] = (digit_hour/10);
                                    Time[0] |= 0x30;
                                    Time[1] = (digit_hour % 10);
                                    Time[1] |= 0x30;
                                  }                                  
                                }
                                else
                                {
                                  if(digit_hour < 10)
                                  {
                                    Time[0] = '0';
                                    Time[1] = (digit_hour | 0x30);
                                  }
                                  else
                                  {
                                    Time[0] = (digit_hour/10);
                                    Time[0] |= 0x30;
                                    Time[1] = (digit_hour % 10);
                                    Time[1] |= 0x30;
                                  }
                                  
                                }

#ifdef DEBUG                              
                                Serial.print(digit_hour,DEC);
                                Serial.print(digit_day,DEC);
                                Serial.print(digit_month,DEC);
                                Serial.print(digit_year,DEC);

                                   
                                Serial.println();
                                Serial.print(Time[0]);
                                Serial.print(Time[1]);
                                Serial.print(Time[2]);
                                Serial.print(Time[3]);
                                Serial.println();

                                Serial.println();
                                Serial.print(Date[0]);
                                Serial.print(Date[1]);
                                Serial.print(Date[2]);
                                Serial.print(Date[3]);
                                Serial.print(Date[4]);
                                Serial.print(Date[5]);
                                Serial.println();

#endif                                

                                Serial.print("Clock ");
                                Serial.print(Time[0]);
                                Serial.print(Time[1]);
                                Serial.print(Time[2]);
                                Serial.print(Time[3]);
                                Serial.print(Date[0]);
                                Serial.print(Date[1]);
                                Serial.print(Date[2]);
                                Serial.print(Date[3]);
                                Serial.print(Date[4]);
                                Serial.println(Date[5]);
                                
                                break;
                              }
                            }



                            
                            Stages = Command_Idle;
                            break;

    case Setup_Command  : 
                            // Use WiFiClientSecure class to create TLS connection

#ifdef DEBUG    
                            Serial.print("connecting to ");
                            Serial.println(host);
#endif
                            if (!httpsclient.connect(host, httpsPort)) 
                            {
                              Serial.println("connection failed");
                              httpsclient.stop();
                              delay(500);
                            }
                            else
                            {
                              Serial.println("connection passed");
                              if (httpsclient.verify(fingerprint, host))
                              {
                                Serial.println("certificate matches");
                              }
                              else
                              {
                                Serial.println("certificate invalid");
                              }
                              Stages = Setup_Sent;    
                            }
                          
                          break;

      case Setup_Sent : 

#ifdef DEBUG
                          Serial.println();
                          Serial.print("\r\nrequesting URL: ");
                          Serial.println(url_Setup + URL_line);
#endif
                          httpsclient.print(String("GET ") + url_Setup + URL_line + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" + "Connection: close\r\n\r\n");
#ifdef DEBUG
                          Serial.println("\r\nrequest sent");
#endif
                          count = 0;
                          //Read and parse all the lines of the reply from server          
                          while (httpsclient.connected()) 
                          {
                            httpReply[count] = httpsclient.read();
#ifdef DEBUG                            
                            Serial.print(httpReply[count]);
#endif
                            if(count == 4096)
                            {
                              count = 0;
                            }
                            else
                            {
                              count++; 
                            }
                    
                          }    

                          Serial.println("\r\nData Received :");

                          Console_line = "";
                          Postal_line = "";
                          Unit_line = "";
                          Day_line = "";
                          Night_line = "";
                          Emergency_line = "";
                          Server_line = "";
                          Main_line = "";
                          IP_line = "";
                          Period_line = "";
                          LightOut_line = "";
                  
                          for(count = 0 ; count < 4096 ; count++)
                          {
                    
                   
                            if(httpReply[count] == 'C' && httpReply[count+1] == 'o' && httpReply[count+2] == 'n' && httpReply[count+3] == 's' && httpReply[count+4] == 'o' && httpReply[count+5] == 'l' && httpReply[count+6] == 'e')
                            {
#ifdef DEBUG                              
                              Serial.println("Console");
#endif                 
                              if(httpReply[(count+15)] == 'x' && httpReply[(count+16)] == '2' && httpReply[(count+17)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Console detect ");
#endif                        
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 18);

                                  if(httpReply[index_count] != 0x5C)
                                  {
                                    Console_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }

                            if(httpReply[count] == 'P' && httpReply[count+1] == 'o' && httpReply[count+2] == 's' && 
                               httpReply[count+3] == 't' && httpReply[count+4] == 'a' && httpReply[count+5] == 'l')
                            {
                              
#ifdef DEBUG                              
                              Serial.println("Postal");
#endif
                              if(httpReply[(count+14)] == 'x' && httpReply[(count+15)] == '2' && httpReply[(count+16)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Postal detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 17);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Postal_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }

                            if(httpReply[count] == 'U' && httpReply[count+1] == 'n' && httpReply[count+2] == 'i' && httpReply[count+3] == 't')
                            {
                              
#ifdef DEBUG                              
                              Serial.println("Unit");
#endif
                              if(httpReply[(count+12)] == 'x' && httpReply[(count+13)] == '2' && httpReply[(count+14)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Unit detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 15);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Unit_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }
                    
                    
                            if(httpReply[count] == 'D' && httpReply[count+1] == 'a' && httpReply[count+2] == 'y')
                            {
                              
#ifdef DEBUG                              
                              Serial.println("Day");
#endif
                              if(httpReply[(count+11)] == 'x' && httpReply[(count+12)] == '2' && httpReply[(count+13)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Day detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 14);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Day_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }
                    
                            if(httpReply[count] == 'N' && httpReply[count+1] == 'i' && httpReply[count+2] == 'g' && httpReply[count+3] == 'h' && httpReply[count+4] == 't')
                            {
#ifdef DEBUG                              
                              Serial.println("Night");
#endif
                              if(httpReply[(count+13)] == 'x' && httpReply[(count+14)] == '2' && httpReply[(count+15)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Night detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 16);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Night_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }

                            if(httpReply[count] == 'E' && httpReply[count+1] == 'm' && httpReply[count+2] == 'e' && httpReply[count+3] == 'r' && httpReply[count+4] == 'g' && httpReply[count+5] == 'e' && 
                               httpReply[count+6] == 'n' && httpReply[count+7] == 'c' && httpReply[count+8] == 'y')
                            {
#ifdef DEBUG                              
                              Serial.println("Emergency");
#endif
                              if(httpReply[(count+17)] == 'x' && httpReply[(count+18)] == '2' && httpReply[(count+19)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Emergency detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 20);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Emergency_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }

                            if(httpReply[count] == 'S' && httpReply[count+1] == 'e' && httpReply[count+2] == 'r' && httpReply[count+3] == 'v' && httpReply[count+4] == 'e' && httpReply[count+5] == 'r')
                            {
#ifdef DEBUG                              
                              Serial.println("Server");
#endif
                              if(httpReply[(count+14)] == 'x' && httpReply[(count+15)] == '2' && httpReply[(count+16)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Server detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 17);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Server_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }

                            if(httpReply[count] == 'M' && httpReply[count+1] == 'a' && httpReply[count+2] == 'i' && httpReply[count+3] == 'n')
                            {
#ifdef DEBUG                              
                              Serial.println("Main");
#endif
                              if(httpReply[(count+12)] == 'x' && httpReply[(count+13)] == '2' && httpReply[(count+14)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Main detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 15);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Main_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }

                            if(httpReply[count] == 'I' && httpReply[count+1] == 'P')
                            {
#ifdef DEBUG                              
                              Serial.println("IP");
#endif
                              if(httpReply[(count+10)] == 'x' && httpReply[(count+11)] == '2' && httpReply[(count+12)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("IP detect ");
#endif
                                for(loop_count = 0 ; loop_count < 20 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 13);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    IP_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }


                            if(httpReply[count] == 'P' && httpReply[count+1] == 'e' && httpReply[count+2] == 'r' && httpReply[count+3] == 'i' && httpReply[count+4] == 'o' && httpReply[count+5] == 'd')
                            {
#ifdef DEBUG                              
                              Serial.println("Period");
#endif
                              if(httpReply[(count+14)] == 'x' && httpReply[(count+15)] == '2' && httpReply[(count+16)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("Period detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 17);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    Period_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }
                              }
                            }                                        

                            if(httpReply[count] == 'L' && httpReply[count+1] == 'i' && httpReply[count+2] == 'g' && httpReply[count+3] == 'h' && httpReply[count+4] == 't' && 
                               httpReply[count+5] == 'O' && httpReply[count+6] == 'u' && httpReply[count+7] == 't')
                            {
#ifdef DEBUG                              
                              Serial.println("LightOut");
#endif
                              if(httpReply[(count+16)] == 'x' && httpReply[(count+17)] == '2' && httpReply[(count+18)] == '2')
                              {
#ifdef DEBUG                              
                                Serial.println("LightOut detect ");
#endif
                                for(loop_count = 0 ; loop_count < 10 ; loop_count++)
                                {
                                  index_count = (loop_count + count + 19);
                          
                                  if((httpReply[index_count]) != 0x5C)
                                  {
                                    LightOut_line += httpReply[index_count];
                                  }
                                  else
                                  {
                                    break;
                                  }
                                }

                                break;
                              }
                            }                                        
                          }

                          line = "";
                          line = "reply was: ";
                          line += Console_line;
                          line += " ";
                          line += Postal_line;
                          line += " ";
                          line += Unit_line;
                          line += " ";
                          line += Day_line;
                          line += " ";
                          line += Night_line;
                          line += " ";
                          line += Emergency_line;
                          line += " ";
                          line += Server_line;
                          line += " ";
                          line += Main_line;
                          line += " ";
                          line += IP_line;
                          line += " ";
                          line += Period_line;
                          line += " ";
                          line += LightOut_line;

                          count = sizeof(line);

                          while(count < 36)
                          {
                            line += ' ';
                            count++;
                          }
                          
                          Serial.println(line);              
                          Serial.println("closing connection");  
                          httpsclient.stop();
                          Stages = Command_Idle;
                        
                        break;
    case Start_Log  :        
                        // Use WiFiClientSecure class to create TLS connection
#ifdef DEBUG                
                        Serial.print("connecting to ");
                        Serial.println(host);
#endif
                        if (!httpsclient.connect(host, httpsPort)) 
                        {
                          Serial.println("connection failed");
                          httpsclient.stop();
                        }
                        else
                        {
                          if (httpsclient.verify(fingerprint, host))
                          {
                            Serial.println("certificate matches");
                          }
                          else
                          {
                            Serial.println("certificate doesn't match");
                          }
                          
                          Stages = Log_Command;    
                        }
                
                        break;


    case Log_Command :  
#ifdef DEBUG       
                        Serial.print("\r\nrequesting URL: ");
                        Serial.println(url_Log + URL_line);
#endif
                        httpsclient.print(String("GET ") + url_Log + URL_line + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: BuildFailureDetectorESP8266\r\n" + "Connection: close\r\n\r\n");

                        Serial.println("\r\nrequest sent");


                        //Read and parse all the lines of the reply from server          
                        while (httpsclient.available()) 
                        {
#ifdef DEBUG                          
                          String line = httpsclient.readStringUntil('\r');
#endif
                        }    
  
                        Serial.println("closing connection");
                        httpsclient.stop();
                        Stages = Command_Idle;
                
                        break;

    default :           break;

  }
                
}

//Check if header is present and correct
bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;  
}

//root page can be accessed only if authentification is ok
void handleRoot(){
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()){
    String header = "HTTP/1.1 301 OK\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  String content = "<html><body><H2>hello, you successfully connected to esp8266!</H2><br>";
  if (server.hasHeader("User-Agent")){
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);
}

void handleLogin(){
  String msg;
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /login\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == Username_line &&  server.arg("PASSWORD") == Password_line ){
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /log\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      Serial.println("Log in Successful");
      return;
    }
  msg = "Wrong username/password! try again.";
  Serial.println("Log in Failed");
  }
  String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
}

void handleLog() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}
