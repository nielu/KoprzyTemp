/*
 Name:		KoprzyTemp.ino
 Created:	3/28/2018 5:38:22 PM
 Author:	janni
*/

#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include <ThingSpeak.h>
#include <Adafruit_Si7021.h>

WiFiClient client;
Adafruit_Si7021 si;

unsigned long tsChannelID = 462382;
const char * privateKey = "P9D8S4FKGQLY6UYT";

void configModeCallback(WiFiManager *myWiFiManager) {
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup()
{
	WiFiManager wifiManager;


	Serial.begin(115200);
	Serial.println("Sensor starting up...");

	Serial.println("Attempting autoconnect...");
	//wifiManager.resetSettings();
	wifiManager.setAPCallback(configModeCallback);
	if (!wifiManager.autoConnect()) {
		Serial.println("failed to connect and hit timeout");
		//reset and try again, or maybe put it to deep sleep
		delay(1000);
		ESP.reset();
	}
	Serial.println("WiFi connected");


	Serial.println("Attempting ThingSpeak connection...");
	if (!ThingSpeak.begin(client)) {
		Serial.println("Failed to connect to ThingSpeak!");
		delay(1000);
		ESP.reset();
	}
	Serial.println("TS connected");

	Serial.println("Starting up SI7021...");
	si.begin();

	Serial.println("startup done");

	
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	char msg[80];
	float temp, hum;
	temp = si.readTemperature();
	hum = si.readHumidity();
	sprintf(msg, "Read temp: %f*C hum: %f%\r\n", temp, hum);
	Serial.print(msg);
	Serial.println("Sending temperature to TS");
	ThingSpeak.writeField(tsChannelID, 1, temp, privateKey);
	Serial.println("Sending humidity to TS");
	ThingSpeak.writeField(tsChannelID, 2, hum, privateKey);
	Serial.println("Sleeping for 15min");
	delay(1000 * 60 * 15);
}
