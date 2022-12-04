#include <Arduino.h>
#include <WiFi.h>
#include "led.h"

void testLeds() {
  initStatusLeds();
  for (int i = 0; i < LED_COUNT * 2; i++) {
    clearLeds();
    int ledIndex = i % 4;
    updateLed(ledIndex, CRGB::Yellow);
    delay(500);
  }
}

// LED 0: ETH initialized? yes -> green, no -> red
// LED 1: MAC address available? yes -> green, no -> red
// LED 2: got IP address? yes -> green, no -> blank
void testEth() {

  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);

  bool ethInit = ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE, true);
  
  // ***** LED 0 *****
  if (!ethInit) {
    Serial.println("ETH init failed");
    updateLed(0, CRGB::Red);
  } else {
    updateLed(0, CRGB::Green);

    // ***** LED 1 *****
    Serial.print("ETH MAC: ");
    Serial.print(ETH.macAddress());
    if (!ETH.macAddress()) {
      updateLed(1, CRGB::Red);
    } else {
      delay(1000);
      updateLed(1, CRGB::Green);
      
      // ***** LED 2 *****
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (!ETH.localIP()) {
        updateLed(2, CRGB::Black);
      } else {
        updateLed(2, CRGB::Green);
      
        if (ETH.fullDuplex()) {
          Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
      }
    }
    Serial.println("");
  }
}

// LED 3: WiFi networks found? yes -> green, no -> has MAC? -> blank, no MAC -> red 
void testWiFi() {
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Starting WiFi scan");
  int numberOfNetworks = WiFi.scanNetworks();
  Serial.println("done");

  Serial.print("WiFi MAC: ");
  Serial.println(WiFi.macAddress());

  if (numberOfNetworks == 0) {
    Serial.println("no networks found");
    if (WiFi.macAddress()) {
      updateLed(3, CRGB::Black);
    } else {
      updateLed(3, CRGB::Red);
    }
  } else {
    Serial.print(numberOfNetworks);
    Serial.println(" networks found");
    // Print SSID and RSSI for each network found
    for (int i = 0; i < numberOfNetworks; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }

    updateLed(3, CRGB::Green);
  }
  Serial.println("");
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  
  // turns each LED on and off
  testLeds();
  clearLeds();

  // initializes LEDs 0, 1, 2
  testEth();
  // initializes LED 3
  testWiFi();
}

void loop() {
  // no-op
}
