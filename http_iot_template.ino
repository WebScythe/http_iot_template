/**
 * TODO: 
 * * save ssid si password pe eeprom
 * * conectare pe baza acelor credentiale
 */

#include "Wire.h"
#include "SerialCommand.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

SerialCommand sCmd;

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif
#define MAX_SSID_LENGTH 32
#define MAX_PASSWD_LENGTH 32

typedef struct Credentials {
    char ssid[MAX_SSID_LENGTH];
    char passwd[MAX_PASSWD_LENGTH];
} Credentials;


ESP8266WebServer server(80);

void setup() {    
    sCmd.addCommand("SET", set_credentials);
    sCmd.setDefaultHandler(help);
    Credentials crds;
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    EEPROM.begin(512);
    EEPROM.get(0, crds);
    Serial.println(crds.ssid);
    Serial.println(crds.passwd);
    WiFi.begin(crds.ssid, crds.passwd);
    Serial.println("");

    int seconds_wait = 10;
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        if (!seconds_wait--) break;
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(crds.ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/plain", "this works without need of authentication");
  });

  server.begin();
}

void loop() {
    sCmd.readSerial();
    server.handleClient();
}

void set_credentials()
{
    char *ssid = sCmd.next();
    char *password = sCmd.next();
    if ( ssid == NULL || password == NULL ) {
        return help("SSID or password not set");
    }

    if (strlen(ssid) > MAX_SSID_LENGTH) {
        return help("SSID must have at most 32 characters.");
    }
    
   if (strlen(password) > MAX_PASSWD_LENGTH) {
        return help("Password must have at most 32 characters.");
    }

    Credentials crds;
   
    strcpy(crds.ssid, ssid);
    strcpy(crds.passwd, password);

    Serial.println("writing values... ");
    Serial.println(crds.ssid);
    Serial.println(crds.passwd);
   
    EEPROM.put(0, crds);
    Credentials crds2;

    EEPROM.commit();
}

void help(const char *command) {
    Serial.println("----------------------------------------------");
    Serial.println(command);
    Serial.println("List of possible commands:");
    Serial.println("* help     - shows this help message");
    Serial.println("* SET,$USER,$PASSWORD");
    Serial.println("----------------------------------------------");
}
