#include <Arduino.h>
#include <TFT_eSPI.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "app.h"

TFT_eSPI tft = TFT_eSPI();

app apli;

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    tft.init();
    tft.setRotation(1);

    apli.init();
}

void loop()
{
    apli.update();

}
