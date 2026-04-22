#include <TFT_eSPI.h>
 
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite petSpr = TFT_eSprite(&tft);
TFT_eSprite uiSpr = TFT_eSprite(&tft);  
 
float energy = 50.0;
float displayEnergy = 50.0; 
float filteredLight = 0;    
long xp = 1000;
int level = 1;
int dynamicGoal = 500;
unsigned long lastTick = 0;
 
uint16_t rainbow[] = {TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN, TFT_CYAN, TFT_BLUE, TFT_MAGENTA};
 
void setup() {
  pinMode(WIO_LIGHT, INPUT);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
 
  long ambient = 0;
  for(int i=0; i<5; i++) { ambient += analogRead(WIO_LIGHT); delay(50); }
  dynamicGoal = (ambient / 5) + 120;
  filteredLight = ambient / 5;
 
  petSpr.createSprite(170, 170);
  uiSpr.createSprite(320, 60);
 
  tft.drawRoundRect(10, 10, 300, 220, 10, TFT_WHITE);
}
 
void playEvolutionSequence() {
  for (int r = 0; r < 2; r++) {
    for (int i = 0; i < 7; i++) {
      petSpr.fillSprite(TFT_BLACK);
      petSpr.fillCircle(85, 85, 75, rainbow[i]); 
      petSpr.drawCircle(85, 85, 75, TFT_WHITE);
      petSpr.setTextColor(TFT_WHITE);
      petSpr.setTextSize(1);
      petSpr.drawString("EVOLUTION!", 50, 163);
      petSpr.pushSprite(75, 15);
      delay(50);
    }
  }
}
 
void playLevelFlash(bool up) {
  tft.fillScreen(up ? TFT_WHITE : TFT_RED);
  delay(60);
  tft.fillScreen(TFT_BLACK);
  tft.drawRoundRect(10, 10, 300, 220, 10, TFT_WHITE);
}
 
void drawPetToSprite(bool charging, bool starving) {
  petSpr.fillSprite(TFT_BLACK);
 
  float breathe = sin(millis() / 400.0) * 3.0;
  int cx = 85;
  int cy = 85;
  int radius = 65 + (int)breathe;
 
  uint16_t bodyCol = TFT_YELLOW;
  if (level >= 50) bodyCol = TFT_RED;          
  else if (level >= 20) bodyCol = TFT_MAGENTA;  
  else if (level >= 15) bodyCol = 0x780F;     
  else if (level >= 10) bodyCol = TFT_CYAN;    
  else if (level >= 5)  bodyCol = TFT_ORANGE;  
 
  if (starving && level < 50) bodyCol = 0x4208; 
 
  if (charging) {
     petSpr.fillCircle(cx, cy, radius + 8, 0xF7BE); 
  }
 
  if (level >= 50) {
    petSpr.fillTriangle(cx-40, cy-40, cx-60, cy-70, cx-20, cy-55, TFT_RED);
    petSpr.fillTriangle(cx+40, cy-40, cx+60, cy-70, cx+20, cy-55, TFT_RED);
    petSpr.drawTriangle(cx-40, cy-40, cx-60, cy-70, cx-20, cy-55, TFT_WHITE);
    petSpr.drawTriangle(cx+40, cy-40, cx+60, cy-70, cx+20, cy-55, TFT_WHITE);
  }
 
  petSpr.fillCircle(cx, cy, radius, bodyCol);
  petSpr.drawCircle(cx, cy, radius, (starving ? TFT_RED : TFT_WHITE));
 
  if (level >= 50) {
    petSpr.setTextColor(TFT_BLACK);
    petSpr.setTextSize(2);
    petSpr.drawString(">", cx - 35, cy - 20);
    petSpr.drawString("<", cx + 15, cy - 20);
    petSpr.drawLine(cx - 25, cy + 30, cx, cy + 45, TFT_BLACK);
    petSpr.drawLine(cx, cy + 45, cx + 25, cy + 30, TFT_BLACK);
  } else if (charging) {
    petSpr.fillCircle(cx - 25, cy - 10, 10, TFT_WHITE);
    petSpr.fillCircle(cx + 25, cy - 10, 10, TFT_WHITE);
    petSpr.fillCircle(cx, cy + 25, 12, TFT_MAROON);
  } else if (starving) {
    petSpr.setTextColor(TFT_BLACK);
    petSpr.setTextSize(3);
    petSpr.drawString("X", cx - 35, cy - 25);
    petSpr.drawString("X", cx + 10, cy - 25);
    petSpr.drawFastHLine(cx - 20, cy + 30, 40, TFT_BLACK);
  } else {
    petSpr.fillCircle(cx - 20, cy - 10, 8, TFT_BLACK);
    petSpr.fillCircle(cx + 20, cy - 10, 8, TFT_BLACK);
    petSpr.drawFastHLine(cx - 15, cy + 35, 31, TFT_BLACK);
    petSpr.drawPixel(cx - 16, cy + 34, TFT_BLACK);
    petSpr.drawPixel(cx + 16, cy + 34, TFT_BLACK);
  }
 
  petSpr.pushSprite(75, 15);
}
 
void updateUI() {
  uiSpr.fillSprite(TFT_BLACK);
 
  displayEnergy += (energy - displayEnergy) * 0.15;
 
  uiSpr.drawRoundRect(10, 10, 300, 20, 5, TFT_WHITE);
  uint16_t barCol = (displayEnergy > 20) ? TFT_GREEN : TFT_RED;
 
  int barWidth = (int)(displayEnergy * 2.96);
  if (barWidth > 0) {
    uiSpr.fillRect(12, 12, barWidth, 16, barCol);
  }
 
  uiSpr.setTextColor(TFT_WHITE);
  uiSpr.setCursor(15, 40);
  uiSpr.printf("LVL: %d | XP: %ld", level, xp);
  uiSpr.pushSprite(0, 180);
}
 
void loop() {
  int rawLight = analogRead(WIO_LIGHT);
  filteredLight = (filteredLight * 0.8) + (rawLight * 0.2);
 
  bool isCharging = (filteredLight > dynamicGoal);
  bool isStarving = (energy <= 0);
 
  if (millis() - lastTick > 100) {
    if (isCharging) {
      energy = min(100.0f, energy + 4.0f);
      xp += (int)filteredLight;
    } else {
      energy = max(0.0f, energy - 1.5f);
    }
 
    if (isStarving && !isCharging) {
      xp = max(0L, xp - 60);
    }
 
    int newLevel = xp / 1000;
    if (newLevel != level) {
      bool up = (newLevel > level);
      playLevelFlash(up);
      if (up && (newLevel % 5 == 0)) {
        playEvolutionSequence();
      }
      level = newLevel;
    }
    lastTick = millis();
  }
 
  drawPetToSprite(isCharging, isStarving);
  updateUI();
}