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
#define CONFIG_LOG_DEFAULT_LEVEL ESP_LOG_DEBUG
//===============================================================================
// Private data for connections are stored in another file 
extern char WIFI_SSID[];      // name of your WiFi network
extern char WIFI_PASSWORD[];  // password to WiFi
extern char HOST_ADDRESS[];   // URL of your AWS endpoint
extern char CLIENT_ID[];      // I put thing name here
extern char TOPIC_NAME[];     // Topic to subscribe to
//===============================================================================
AWS_IOT hornbill;
int status = WL_IDLE_STATUS;
int tick=0,msgCount=0,msgReceived = 0;
char payload[512];
char rcvdPayload[512];
//===============================================================================
void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(rcvdPayload, payLoad, payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}
//===============================================================================
// the setup routine runs once when M5Stack starts up
void setup()
{
  int i = 0;

  ESP_LOGI("test", "TEST");
  M5.begin();
  M5.Lcd.clear(BLUE);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setFreeFont(FM12);

  M5.Lcd.setCursor(0, 20);
  M5.Lcd.print("WiFi connect");
  while (status != WL_CONNECTED)
  {
    M5.Lcd.setCursor(160, 20);
    M5.Lcd.print(i++, HEX);
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(5000);
  }

  M5.Lcd.clear(BLUE);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.print("WiFi connected");
  vTaskDelay(3000 / portTICK_RATE_MS);
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.print(WiFi.localIP());
  i = hornbill.connect(HOST_ADDRESS, CLIENT_ID);

  if ( i== 0)
  {
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.print("AWS connected");

    i = hornbill.subscribe(TOPIC_NAME, mySubCallBackHandler);
    if (i == 0)
    {
            M5.Lcd.setCursor(0, 60);
            M5.Lcd.print("Subscribed");
    }
    else
    {
            M5.Lcd.setCursor(0, 60);
            M5.Lcd.print("NOT Subscribed");
    }
  }
  else
  {
    M5.Lcd.setCursor(0, 40);
    M5.Lcd.print("AWS connected FAILED");
  }
}
//===============================================================================
// the loop routine runs over and over again forever
void loop() 
{
  int i;
  int ypos = 20;
    if(msgReceived == 1)
    {
        msgReceived = 0;
        M5.Lcd.setCursor(160, ypos);
        M5.Lcd.print(rcvdPayload);
        ypos = ypos+20;
        if (ypos > 240)
        {
          ypos = 20;
        }
    }
    vTaskDelay(1000 / portTICK_RATE_MS); 
    tick++;

  // DEBUG
  M5.update();
  if (M5.BtnA.wasReleased())
  {   
    M5.Lcd.clear(BLUE);
  }
  if (M5.BtnB.wasReleased())
  {   
    i = hornbill.subscribe(TOPIC_NAME, mySubCallBackHandler);
    if (i == 0)
    {
            M5.Lcd.setCursor(0, 60);
            M5.Lcd.print("Subscribed");
    }
    else
    {
            M5.Lcd.setCursor(0, 60);
            M5.Lcd.print("NOT Subscribed");
    }
  }
  if (M5.BtnC.wasReleased())
  {   
  }
}
