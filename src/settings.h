#pragma once

#include <ETH.h>

// Ethernet settings
#define ETH_CLK_MODE    ETH_CLOCK_GPIO16_OUT
#define ETH_POWER_PIN   5
#define ETH_TYPE        ETH_PHY_LAN8720
#define ETH_ADDR        0
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18

#define ETH_TIMEOUT_MILLIS 10000

// LED Settings
#define ESP_LED_PIN 32
#define LED_COUNT   4
#define CHANNEL     0
