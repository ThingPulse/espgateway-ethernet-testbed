#include <Arduino.h>
#include <WiFi.h>
#include "led.h"

bool ethernetInit() {


    pinMode(ETH_POWER_PIN, OUTPUT);
    digitalWrite(ETH_POWER_PIN, HIGH);

    bool result = ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE, true);
    if (!result) {
        Serial.println("ETH Init failed");
        updateLed(0, CRGB::Red);
        return false;
    }

    updateLed(0, CRGB::Green);
    Serial.print("ETH MAC: ");
    Serial.print(ETH.macAddress());
    if (!ETH.macAddress()) {
      updateLed(1, CRGB::Red);
      return false;
    }
    delay(1000);
    updateLed(1, CRGB::Green);
    Serial.print(", IPv4: ");
    Serial.print(ETH.localIP());
    if (!ETH.linkUp()) {
      updateLed(2, CRGB::Black);
      return false;
    }
    updateLed(2, CRGB::Green);
    if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
    }
    Serial.print(", ");
    Serial.print(ETH.linkSpeed());
    Serial.println("Mbps");
    return true;
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  initStatusLeds();
  for (int i = 0; i < LED_COUNT * 2; i++) {
    clearLeds();
    int ledIndex = i % 4;
    updateLed(ledIndex, CRGB::Yellow);
    delay(500);
  }
  clearLeds();
  ethernetInit();
}

void loop() {

}