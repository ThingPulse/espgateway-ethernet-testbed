#include <Arduino.h>
#include <WiFi.h>
#include "led.h"
#include <ArduinoJson.h>

bool ethInit = false;
uint32_t psramSize = 0;
uint32_t freePsram = 0;
bool isIpDefined = false;

void addTestMessage(JsonArray& data, String name, String value, String result) {
    JsonObject object = data.createNestedObject();
    object["name"] = name;
    object["value"] = value;
    object["result"] = result;
}

void testDevice() {
  StaticJsonDocument<1024> doc;
  JsonArray data = doc.to<JsonArray>();

  addTestMessage(data, "Mac Address", String(WiFi.macAddress()), "OK" );
  addTestMessage(data, "Chip Model", String(ESP.getChipModel()), "OK" );
  addTestMessage(data, "Chip Revision", String(ESP.getChipRevision()), "OK");
  addTestMessage(data, "Available Cores", String(ESP.getChipCores()), ESP.getChipCores() == 2 ? "OK" : "NOK");
  addTestMessage(data, "Heap Size", String(ESP.getHeapSize() / 1024) + "kb", ESP.getHeapSize() > 100000 ? "OK" : "NOK");
  addTestMessage(data, "Free Heap", String(ESP.getFreeHeap() / 1024) + "kb", ESP.getFreeHeap() > 100000 ? "OK" : "NOK");
  addTestMessage(data, "PSRAM Size", String(psramSize == 0 ? 0 : psramSize / 1024) + "kb", psramSize == 4192139 ? "OK" : "NOK");
  addTestMessage(data, "Free PSRAM", String(freePsram / 1024) + "kb", freePsram == 4192139 ? "OK" : "NOK");
  addTestMessage(data, "ETH MAC Address", String(ETH.macAddress()), ETH.macAddress() ? "OK" : "NOK");
  addTestMessage(data, "ETH Local IP", ETH.localIP().toString(), ETH.localIP() ? "OK" : "NOK");
  addTestMessage(data, "ETH Full Duplex", ETH.fullDuplex() ? "true" : "false", ETH.fullDuplex() ? "OK" : "NOK");
  addTestMessage(data, "ETH Link Speed", String(ETH.linkSpeed()) + "Mbs", ETH.linkSpeed() ? "OK" : "NOK");
  addTestMessage(data, "Build Date", String(__DATE__), "OK");
  addTestMessage(data, "Build Time", String(__TIME__), "OK");
  serializeJson(doc, Serial);
  Serial.println();
}

void testLeds() {
  initStatusLeds();
  for (int i = 0; i < LED_COUNT * 2; i++) {
    clearLeds();
    int ledIndex = i % 4;
    updateLed(ledIndex, CRGB::Yellow);
    delay(500);
  }
}



void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
    isIpDefined = true;
}


void initEth() {
  pinMode(ETH_POWER_PIN, OUTPUT);
  digitalWrite(ETH_POWER_PIN, HIGH);

  ethInit = ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE, true);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_ETH_GOT_IP);
    
}

// LED 0: ETH initialized? yes -> green, no -> red
// LED 1: MAC address available? yes -> green, no -> red
// LED 2: got IP address? yes -> green, no -> blank
void testEth() {


  
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
  // Get PSRAM before initializing Ethernet
  // Due to bug: https://github.com/espressif/arduino-esp32/issues/6389
  psramSize = ESP.getPsramSize();
  freePsram = ESP.getFreePsram();
  
  // turns each LED on and off
  testLeds();
  clearLeds();

  initEth();

  
  // initializes LEDs 0, 1, 2
  //testEth();
  // initializes LED 3
  //testWiFi();
}

void loop() {
  if (isIpDefined || millis() > ETH_TIMEOUT_MILLIS) {
    testDevice();
  }
  delay(1000);
}
