#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Matter.h>

#define LED_PIN        26
#define NUM_LEDS       39
#define GLOBAL_BRIGHT  200

const char *ssid = "<>";
const char *pass = "<>";

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
MatterEnhancedColorLight lightEP;

espRgbColor_t lastRGB  = {255, 255, 255};
uint8_t       lastBri  = 255;  

void applyState()
{
  if (!lightEP.getOnOff()) {
    strip.clear();
    strip.show();
    return;
  }
  espRgbColor_t rgb = lightEP.getColorRGB();
  uint8_t       bri = lightEP.getColorHSV().v;      // 0–255

  if (bri == 1 && rgb.r == 1 && rgb.g == 0 && rgb.b == 0) {
    return;
  }

  if (bri == 1 && rgb.r == 0 && rgb.g == 1 && rgb.b == 0) {
    return;
  }

  if (bri == 1 && rgb.r == 0 && rgb.g == 0 && rgb.b == 1) {
    return;
  }

  Serial.printf("RGB: %3u, %3u, %3u\n", rgb.r, rgb.g, rgb.b);
  Serial.printf("BRI: %3u\n", bri);

  strip.setBrightness(bri);
  strip.fill(strip.Color(rgb.r, rgb.g, rgb.b));
  strip.show();
}


void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(GLOBAL_BRIGHT);
  strip.show();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(200);

  lightEP.begin(false);

  lightEP.onChange([](bool, HsvColor_t, uint8_t, uint16_t) -> bool {
    applyState();
    return true;
  });

  Matter.begin();

  if (!Matter.isDeviceCommissioned()) {
    Serial.printf("\nPair-code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR-URL  : %s\n\n",  Matter.getOnboardingQRCodeUrl().c_str());
  }
}

void loop() {
  delay(10);
}
