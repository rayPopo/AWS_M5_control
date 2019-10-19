// Arduino M5Stack sketch to connect to AWS and subscribe for messages in a topic 
// Created mainly as debug monitor for another IOT devices by subscription to a topic
// Plan to publish to the topic later. 
//
// Need one more .ino file with credentials (see Private data variables below) and 
// Amazon certificates in aws_iot_certficates.c (see Hornbill AWS_IOT for details and example)
// Don't forget to activate your certs/policies and link it all together with a thing 
// (Otherwise you may observe AWS errors via Serial port monitor to debug connection problems)
//
// With M5Stack support library from https://github.com/m5stack/M5Stack 
// and AWS_IOT library from https://github.com/ExploreEmbedded/Hornbill-Examples/tree/master/arduino-esp32/AWS_IOT
//===============================================================================
#include <M5Stack.h>
#include <Free_Fonts.h> 
#include <AWS_IOT.h>
#include <WiFi.h>
#define ESP32
#define MAX_RETRIES 128
//===============================================================================
// Private data for connections are stored in another file 
extern char WIFI_SSID[];      // name of your WiFi network
extern char WIFI_PASSWORD[];  // password to WiFi
extern char HOST_ADDRESS[];   // URL of your AWS endpoint
extern char CLIENT_ID[];      // I put thing name here
extern char TOPIC_NAME[];     // Topic to subscribe to
//===============================================================================
// Global variables
AWS_IOT aws_client;
int tick=0, msgCount=0, msgReceived = 0;
char SendBuffer[1024];
char ReceiveBuffer[1024];
//===============================================================================
void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(ReceiveBuffer, payLoad, payloadLen);
    ReceiveBuffer[payloadLen] = 0;
    msgReceived = 1;
}
//-------------------------------------------------------------------------------
void DoReset(char *Message)
{
  M5.Lcd.clear(BLUE);
  M5.Lcd.setTextColor(TFT_RED); 
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.print(Message);
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.print("Rebooting in 60 sec...");

  vTaskDelay(60000 / portTICK_RATE_MS);
  ESP.restart();
}
//-------------------------------------------------------------------------------
void ConnectWiFi()
{
  int i = 0;
  int retry_cnt = 0;
  
  M5.Lcd.clear(TFT_BLUE);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setFreeFont(FM12);

  strcpy(SendBuffer, "WiFi SSID: ");
  strcat(SendBuffer, WIFI_SSID);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.print(SendBuffer);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for radio connection setup
  while (WiFi.status() != WL_CONNECTED)
  {
    retry_cnt++;
    if (retry_cnt > MAX_RETRIES)
      DoReset("WiFi failed");
    
    for (i=0; i<retry_cnt; i++)
      SendBuffer[i] = '.';
    SendBuffer[retry_cnt] = 0;    
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.print(SendBuffer);
    vTaskDelay(1000 / portTICK_RATE_MS); // sleep for 1 second
  }

  M5.Lcd.clear(BLUE);
  M5.Lcd.setTextColor(TFT_GREEN);
  M5.Lcd.setCursor(0, 20);
  strcpy(SendBuffer, "WiFi SSID: ");
  strcat(SendBuffer, WIFI_SSID);
  M5.Lcd.print(SendBuffer);

  // Wait for DHCP IP assignment
  retry_cnt = 0;
  M5.Lcd.setTextColor(TFT_WHITE);
  while (WiFi.localIP()[0] == 0)
  {
    retry_cnt++;
    if (retry_cnt > MAX_RETRIES)
      DoReset("DHCP failed");
    M5.Lcd.setCursor(0, 40);
    strcpy(SendBuffer, "IP addr: ");
    strcat(SendBuffer, WiFi.localIP().toString().c_str());
    M5.Lcd.print(SendBuffer); 
    vTaskDelay(1000 / portTICK_RATE_MS); // sleep for 1 second
  }
  M5.Lcd.fillRect(0, 21, 320, 20, TFT_BLUE);
  M5.Lcd.setTextColor(TFT_GREEN);
  M5.Lcd.setCursor(0, 40);
  strcpy(SendBuffer, "IP addr: ");
  strcat(SendBuffer, WiFi.localIP().toString().c_str());
  M5.Lcd.print(SendBuffer);  
}
//===============================================================================
// the setup routine runs once when M5Stack starts up
void setup()
{
  int i = 0;
  int retry_cnt = 0;

  M5.begin();
  
//
//  vTaskDelay(30000 / portTICK_RATE_MS);
//  i = aws_client.connect(HOST_ADDRESS, CLIENT_ID);
//
//  if ( i== 0)
//  {
//    M5.Lcd.setCursor(0, 40);
//    M5.Lcd.print("AWS connected");
//
//    i = aws_client.subscribe(TOPIC_NAME, mySubCallBackHandler);
//    if (i == 0)
//    {
//            M5.Lcd.setCursor(0, 60);
//            M5.Lcd.print("Subscribed");
//    }
//    else
//    {
//            M5.Lcd.setCursor(0, 60);
//            M5.Lcd.print("NOT Subscribed");
//    }
//  }
//  else
//  {
//    M5.Lcd.setCursor(0, 40);
//    M5.Lcd.print("AWS connected FAILED");
//  }
}
//===============================================================================
// the loop routine runs over and over again forever
void loop() 
{
  if (WiFi.status() != WL_CONNECTED)
    ConnectWiFi();
//  int i;
//  int ypos = 20;
//    if(msgReceived == 1)
//    {
//        msgReceived = 0;
//        M5.Lcd.setCursor(160, ypos);
//        M5.Lcd.print(ReceiveBuffer);
//        ypos = ypos+20;
//        if (ypos > 240)
//        {
//          ypos = 20;
//        }
//    }
//    vTaskDelay(1000 / portTICK_RATE_MS); 
//    tick++;
//
//  // DEBUG
//  M5.update();
//  if (M5.BtnA.wasReleased())
//  {   
//    M5.Lcd.clear(BLUE);
//  }
//  if (M5.BtnB.wasReleased())
//  {   
//    i = aws_client.subscribe(TOPIC_NAME, mySubCallBackHandler);
//    if (i == 0)
//    {
//            M5.Lcd.setCursor(0, 60);
//            M5.Lcd.print("Subscribed");
//    }
//    else
//    {
//            M5.Lcd.setCursor(0, 60);
//            M5.Lcd.print("NOT Subscribed");
//    }
//  }
//  if (M5.BtnC.wasReleased())
//  {   
//  }
}
