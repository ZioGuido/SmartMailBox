/////////////////////////////////////////////////////////////////////////////////
// Smart Mail Box project
// by Guido Scognamiglio - 2019
//
// This sketch just runs once, posts a message to a feed and
// enters the Deep Sleep mode for low power consumption.
//
// It's basically a modified version of the Adafruit MQTT example sketch.
//
// See the attached schematics to see how I wake up the ESP01 module
// using an Hall-effect IC and an NPN transistor to trigger the RESET pin.
//

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <WiFiUdp.h>
#include <NTPClient.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "{your SSID goes here}"
#define WLAN_PASS       "{your password goes here}"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "{your Adafruit.io username goes here}"
#define AIO_KEY         "{your Adafruit.io key goes here}"

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// NTP Client for internet time
#define NTP_SERVER "3.it.pool.ntp.org" // Change to your preference
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER);

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish mailbox = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/{my feed name goes here}");

// Code from: http://forum.arduino.cc/index.php?topic=329079.0
// Zone is CET (GMT+1), DST starts last sunday of march at 2:00 and ends last sunday of october at 2:00 (3:00)
// You might want to modify this function if you're in a different timezone, or get rid of it if your Country doesn't use the DST
bool CheckDST()
{
  bool dst = false;
  int thisMonth = timeClient.getMonth();
  int thisDay = timeClient.getDate();
  int thisWeekday = timeClient.getDay();
  int thisHour = timeClient.getHours();

  if (thisMonth == 10 && thisDay < (thisWeekday + 24)) dst = true;
  if (thisMonth == 10 && thisDay > 24 && thisWeekday == 1 && thisHour < 2) dst = true;
  if (thisMonth < 10 && thisMonth > 3) dst = true;
  if (thisMonth == 3 && thisDay > 24 && thisDay >= (thisWeekday + 24))
    if (!(thisWeekday == 1 && thisHour < 2))
      dst = true;

  return dst;
}

void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  {
     mqtt.disconnect();
     delay(5000);  // wait 5 seconds
     retries--;
     if (retries == 0) {
       // basically die and wait for WDT to reset me
       while (1);
     }
  }
}

void setup()
{
  // Connect to WiFi access point
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  // Get current time from NTP
  timeClient.begin();
  if (timeClient.update()) timeClient.setTimeOffset(CheckDST() ? 3600*2 : 3600);

  // Format the string to post to MQTT feed (change to your preference)
  String txtToPost = "Posta alle ore " + timeClient.getFormattedTime();

  // Connect to MQTT broker
  MQTT_connect();

  // Publish to feed
  mailbox.publish(txtToPost.c_str());

  // Wait 10 seconds
  delay(10000);

  // Enter Deep Sleep Mode until the Reset pin is triggered
  ESP.deepSleep(0);
}

// Nothing to do here...
void loop()
{
}
