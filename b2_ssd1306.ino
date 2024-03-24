void init_display() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  ESP_LOGV("SSD1306", "display.began");

  //display.display();
  //delay(1000);

  display.clearDisplay();  // Clear the buffer
  display.cp437(true);     // Use full 256 char 'Code Page 437' font
  display.setTextWrap(false);
#ifdef SPLASHSCREEN_TIMEOUT
  printVersion();
  display.display();
  delay(SPLASHSCREEN_TIMEOUT);
#endif
  display.setTextSize(1);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.setCursor(0, 0);      // Start at top-left corner
  //display.invertDisplay(true);  // invert display image, no need to display.display
  display.clearDisplay();
  display.display();
}

void printVersion() {
  display.fillRect(0, 0, 128, 17, WHITE);
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(5, 1);
  display.println("PowerLoop2");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 18);
  display.print("r");
  display.print(REV_NO);
  display.print(", ");
  display.println(REV_DATE);
  display.setCursor(0, 8 * 4);
  display.setTextWrap(true);
  display.fillRect(0, 26, 128, 4, WHITE);
  display.println(SPLASH_MESSAGE);
  display.setTextWrap(false);
}