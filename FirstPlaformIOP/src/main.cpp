#include "Arduino.h"
#include <ArduinoJson.h>
#include <Nextion.h>
#include "NexButton.h"
#include "NexText.h"
#include <Http.h>
#include <SMS.h>

#define LEVEL_HIGH (30)
#define LEVEL_LOW (0)

#define CH0_OFFSET (40 - LEVEL_HIGH / 2)
#define CH1_OFFSET (CH0_OFFSET + 40 * 1)
#define CH2_OFFSET (CH0_OFFSET + 40 * 2)
#define CH3_OFFSET (CH0_OFFSET + 40 * 3)

unsigned int RX_PIN = A9;
unsigned int TX_PIN = A8;
unsigned int RST_PIN = 12;

SMS gprs(9600, RX_PIN, TX_PIN, RST_PIN);
NexButton b0 = NexButton(0, 1, "b0");
NexButton b1 = NexButton(0, 3, "b1");
NexButton b2 = NexButton(0, 4, "b2");
NexButton b3 = NexButton(0, 5, "b3");

char buffer[10] = {0};

NexTouch *nextion_touch_events[] =
    {
        &b0,
        &b1,
        &b2,
        &b3,
        NULL};

void b0PopCallback(void *ptr)
{
  Serial.println("GPRS - Send SMS Test ...");
  delay(1000);
  while (0 != gprs.init())
  {
    delay(1000);
    Serial.print("init error\r\n");
  }
  Serial.println("Init success, start to send SMS message...");
  gprs.sendSMS("0814675538", "2296518502569135"); //define phone number and text
}

void DataPlot()
{

  NexWaveform s0 = NexWaveform(0, 4, "s0");

  static uint8_t ch0_data = LEVEL_LOW;
  static uint8_t ch1_data = LEVEL_LOW;
  static uint8_t ch2_data = LEVEL_LOW;
  static uint8_t ch3_data = LEVEL_LOW;
  static uint32_t started = 0;

  if (millis() - started >= 2000)
  {
    started = millis();
    if (LEVEL_HIGH == ch0_data)
    {
      ch0_data = LEVEL_LOW;
    }
    else
    {
      ch0_data = LEVEL_HIGH;
    }
  }

  ch1_data = ch0_data + random(0, 2);
  ch2_data = ch0_data + random(0, 5);
  ch3_data = ch0_data + random(0, 8);

  s0.addValue(0, CH0_OFFSET + ch0_data);
  s0.addValue(1, CH1_OFFSET + ch1_data);
  s0.addValue(2, CH2_OFFSET + ch2_data);
  s0.addValue(3, CH3_OFFSET + ch3_data);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  nexInit();

  b0.attachPop(b0PopCallback, &b0);
  gprs.preInit();
}

void loop()
{
  nexLoop(nextion_touch_events);
  DataPlot();
  String Test = gprs.searchNetwork(5000);
  Serial.println(Test);
}