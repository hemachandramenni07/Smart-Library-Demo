/*
  FIND MY BOOK - AI & IoT Smart Library
  Converted from .cpp to .c container format.
  NOTE: This program uses Arduino C++ libraries (Adafruit classes, MFRC522,
  String objects, constructors, methods). A fully pure-C version requires
  rewriting those libraries and replacing C++ objects with C structs/functions.
*/

/*
  ==========================================================================
   FIND MY BOOK - AI & IoT Smart Library  (combined project, ESP32-S3)
  ==========================================================================
   This one firmware merges the three earlier Wokwi circuits:

   1. Touch-screen kiosk   (was: touchscreen)   ILI9341 + FT6206 capacitive touch
   2. RFID issue / return  (was: DE_casestudy)  desk reader + door (anti-theft) reader,
                                                 WS2812 LED strip, buzzer
   3. Shelf navigation     (was: c1)            4 shelf sensors, 4 shelf LEDs,
                                                 SSD1306 OLED, ARM button, Serial commands

   ---------------- Pin map (ESP32-S3-DevKitC-1) ----------------
   SPI bus (shared)   SCK 12, MOSI 11, MISO 13
     ILI9341 TFT      CS 10, D/C 8, RST 9
     RFID desk        SDA(SS) 14, RST 21
     RFID door        SDA(SS) 47, RST 21 (shared)
   I2C bus (shared)   SDA 6, SCL 7
     FT6206 touch     0x38
     SSD1306 OLED     0x3C
   Shelf sensors      4, 5, 15, 16   (slide switch, LOW = book on shelf)
   Shelf LEDs         17, 18, 38, 39
   Buzzer             40
   ARM push button    41   (night / anti-theft mode)
   WS2812 LED strip   42   (8 pixels, gate status light)

   ---------------- RFID cards (Wokwi presets) ----------------
   Click an RFID reader in Wokwi, pick a card, press "Tap".
     Blue  01:02:03:04 -> Mathematics      Yellow 55:66:77:88 -> History
     Green 11:22:33:44 -> Physics          Red    AA:BB:CC:DD -> C Programming
     Key Fob C0:FF:EE:99 -> not a library book (UNKNOWN)

   ---------------- Serial Monitor (115200, newline) ----------------
     HELP | LIST | SEARCH <text> | CLEAR | ARM | DISARM | REPORT | SYNC
   RFID events are also printed as  ISSUED,<uid>  RETURNED,<uid>
                                    THEFT,<uid>   EXIT_OK,<uid>   UNKNOWN,<uid>
  ==========================================================================
*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <MFRC522.h>

// ======================= Settings (change here) =======================
const bool TOUCH_FLIP_X        = true;    // Wokwi/real FT6206: origin is bottom-right
const bool TOUCH_FLIP_Y        = true;
const bool PRINT_TOUCH         = true;    // print touch coordinates to Serial
const unsigned long NAV_TIMEOUT_MS    = 20000;  // shelf LED blinks this long
const unsigned long SCAN_TIMEOUT_MS   = 20000;  // kiosk waits this long for a card
const unsigned long RESULT_SCREEN_MS  = 4000;   // success/error screen then home
const unsigned long DEBOUNCE_MS       = 60;
const uint8_t       STRIP_BRIGHTNESS  = 80;

// ======================= Pins =======================
#define SPI_SCK      12
#define SPI_MOSI     11
#define SPI_MISO     13

#define TFT_CS       10
#define TFT_DC        8
#define TFT_RST       9

#define SS_PIN_DESK  14
#define SS_PIN_DOOR  47
#define RFID_RST     21

#define I2C_SDA       6
#define I2C_SCL       7

#define BUZZER_PIN   40
#define ARM_BTN_PIN  41
#define STRIP_PIN    42
#define STRIP_LEDS    8

// ======================= Devices =======================
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_FT6206  touch;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
Adafruit_NeoPixel strip(STRIP_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);
MFRC522 deskReader(SS_PIN_DESK, RFID_RST);
MFRC522 doorReader(SS_PIN_DOOR, MFRC522::UNUSED_PIN);   // RST shared with desk reader

bool oledOK  = false;
bool touchOK = false;

// ======================= Book database =======================
struct Book {
  const char* title;
  const char* category;
  const char* sku;
  const char* location;
  byte uid[4];          // RFID tag UID (Wokwi card presets)
  const char* card;     // which Wokwi card to use
  int sensorPin;        // shelf sensor
  int ledPin;           // shelf LED
  bool issued;          // issued to a member (RFID desk)
  bool onShelf;         // debounced shelf sensor
  bool lastRaw;
  unsigned long lastChange;
  int issues;           // analytics
  int searches;
  int picks;
};

Book books[] = {
  { "Mathematics",   "Science",   "M101", "Rack A - Shelf 1", {0x01,0x02,0x03,0x04}, "Blue",   4, 17, false, true, true, 0, 0, 0, 0 },
  { "Physics",       "Science",   "P201", "Rack A - Shelf 2", {0x11,0x22,0x33,0x44}, "Green",  5, 18, false, true, true, 0, 0, 0, 0 },
  { "History",       "Arts",      "H301", "Rack B - Shelf 1", {0x55,0x66,0x77,0x88}, "Yellow",15, 38, false, true, true, 0, 0, 0, 0 },
  { "C Programming", "Computing", "C401", "Rack B - Shelf 2", {0xAA,0xBB,0xCC,0xDD}, "Red",   16, 39, false, true, true, 0, 0, 0, 0 }
};
const int NUM_BOOKS = sizeof(books) / sizeof(books[0]);

// ======================= Kiosk screens =======================
enum Screen {
  HOME, FIND_BOOK, ISSUE_BOOK, RETURN_BOOK, ABOUT_US,
  BOOK_DETAILS, ISSUE_SCAN, RETURN_SCAN, SUCCESS, ERROR_SCREEN
};
Screen currentScreen = HOME;
int  detailsBook = -1;
unsigned long scanStart = 0;
unsigned long resultShownAt = 0;
int  lastCountdown = -1;
bool wasTouched = false;

// ======================= State =======================
bool armed = false;                 // night / anti-theft mode
bool alarmActive = false;
unsigned long alarmUntil = 0;       // 0 = until DISARM
int  alarmBook = -1;
unsigned long lastAlarmToggle = 0;
bool alarmHigh = false;

uint8_t navMask = 0;                // bit i = navigating to book i
unsigned long navStart = 0;
unsigned long lastBlink = 0;
bool blinkState = false;

bool lastBtn = HIGH;
unsigned long lastBtnChange = 0;

unsigned long stripRevertAt = 0;
unsigned long lastOledRefresh = 0;

String oledLine1 = "Smart Library";
String oledLine2 = "";
String oledLine3 = "";

// ======================= Forward declarations =======================
void drawHome();
void drawFindBook();
void drawIssueBook();
void drawReturnBook();
void drawAboutUs();
void drawBookDetails(int i);
void drawScanScreen(bool issue);
void drawResult(bool ok, const String& title, const String& l1, const String& l2, const String& l3);
void drawHeader(const String& title, uint16_t color = ILI9341_BLUE);
void drawButton(int x, int y, int w, int h, const String& text, uint16_t color = ILI9341_DARKCYAN);
void drawHomeButton(const char* label = "HOME");
void handleTouch(int x, int y);
void setIdleOled();
void showOled();
void startNavigation(int i);
void clearNav();
void startAlarm(int bookIdx, unsigned long durationMs);
void stopAlarm();
void printSync(int i);

// ======================= Small helpers =======================
String lower(String s) { s.toLowerCase(); return s; }

void beep(int freq, int ms) { tone(BUZZER_PIN, freq, ms); }

void setStrip(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < STRIP_LEDS; i++) strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
}
void stripIdle() { setStrip(0, 0, 20); }                 // dim blue
void stripFlash(uint8_t r, uint8_t g, uint8_t b, unsigned long ms) {
  setStrip(r, g, b);
  stripRevertAt = millis() + ms;
}

int countIssued()   { int c = 0; for (int i = 0; i < NUM_BOOKS; i++) if (books[i].issued) c++; return c; }
int countOffShelf() { int c = 0; for (int i = 0; i < NUM_BOOKS; i++) if (!books[i].onShelf) c++; return c; }

String uidToString(MFRC522& r) {
  String s;
  for (byte i = 0; i < r.uid.size; i++) {
    if (r.uid.uidByte[i] < 0x10) s += '0';
    s += String(r.uid.uidByte[i], HEX);
  }
  s.toUpperCase();
  return s;
}

int findBookByUid(MFRC522& r) {
  if (r.uid.size != 4) return -1;
  for (int i = 0; i < NUM_BOOKS; i++)
    if (memcmp(r.uid.uidByte, books[i].uid, 4) == 0) return i;
  return -1;
}

const char* statusText(int i) {
  if (books[i].issued)   return "ISSUED";
  if (!books[i].onShelf) return "OFF SHELF";
  return "AVAILABLE";
}

// ======================= OLED (shelf status) =======================
void showOled() {
  if (!oledOK) return;
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println(oledLine1);
  oled.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  oled.setCursor(0, 16);
  oled.println(oledLine2);
  oled.setCursor(0, 30);
  oled.println(oledLine3);
  oled.setCursor(0, 54);
  oled.print(armed ? "ANTI-THEFT: ARMED" : "ANTI-THEFT: OFF");
  oled.display();
}

void setIdleOled() {
  oledLine1 = "Smart Library";
  oledLine2 = "Use kiosk or SEARCH";
  oledLine3 = "Issued:" + String(countIssued()) + "  Off shelf:" + String(countOffShelf());
  showOled();
}

// ======================= Shelf navigation =======================
void startNavigation(int i) {
  navMask = (1 << i);
  navStart = millis();
  books[i].searches++;
  oledLine1 = "Go to:";
  oledLine2 = String(books[i].title) + " (" + books[i].sku + ")";
  oledLine3 = String(books[i].location) + "\n" + statusText(i);
  showOled();
  beep(1200, 120);
}

void clearNav() {
  navMask = 0;
  for (int i = 0; i < NUM_BOOKS; i++) digitalWrite(books[i].ledPin, LOW);
  if (!alarmActive) setIdleOled();
}

void searchItems(String q) {
  q.trim();
  if (q.length() == 0) { Serial.println("Usage: SEARCH <title | category | SKU>"); return; }
  String key = lower(q);
  navMask = 0;
  int found = 0, first = -1;

  for (int i = 0; i < NUM_BOOKS; i++) {
    bool hit = lower(books[i].title).indexOf(key) >= 0 ||
               lower(books[i].category).indexOf(key) >= 0 ||
               lower(books[i].sku).indexOf(key) >= 0;
    if (hit) {
      navMask |= (1 << i);
      books[i].searches++;
      found++;
      if (first < 0) first = i;
      Serial.printf("FOUND: %s (%s, SKU %s) -> %s | %s\n",
                    books[i].title, books[i].category, books[i].sku,
                    books[i].location, statusText(i));
    }
  }

  if (found == 0) {
    Serial.println("No matching book found.");
    oledLine1 = "Search: " + q;
    oledLine2 = "Not found";
    oledLine3 = "";
    showOled();
    beep(300, 400);
    return;
  }

  navStart = millis();
  oledLine1 = "Go to:";
  if (found == 1) {
    oledLine2 = String(books[first].title) + " (" + books[first].sku + ")";
    oledLine3 = String(books[first].location) + "\n" + statusText(first);
    if (currentScreen != ISSUE_SCAN && currentScreen != RETURN_SCAN) drawBookDetails(first);
  } else {
    oledLine2 = String(found) + " books found";
    oledLine3 = "Follow blinking LEDs";
  }
  showOled();
  beep(1200, 120);
}

void updateNavLEDs() {
  if (navMask == 0) return;
  if (millis() - navStart > NAV_TIMEOUT_MS) {
    clearNav();
    Serial.println("Navigation timed out.");
    return;
  }
  if (millis() - lastBlink > 300) {
    lastBlink = millis();
    blinkState = !blinkState;
    for (int i = 0; i < NUM_BOOKS; i++) {
      if (alarmActive && i == alarmBook) continue;
      digitalWrite(books[i].ledPin, (navMask & (1 << i)) ? blinkState : LOW);
    }
  }
}

// ======================= Alarm (shelf theft + door theft) =======================
void startAlarm(int bookIdx, unsigned long durationMs) {
  alarmActive = true;
  alarmBook = bookIdx;
  alarmUntil = durationMs ? millis() + durationMs : 0;
}

void stopAlarm() {
  alarmActive = false;
  if (alarmBook >= 0 && !(navMask & (1 << alarmBook))) digitalWrite(books[alarmBook].ledPin, LOW);
  alarmBook = -1;
  noTone(BUZZER_PIN);
  stripIdle();
  if (navMask == 0) setIdleOled();
}

void handleAlarm() {
  if (!alarmActive) return;
  if (alarmUntil != 0 && (long)(millis() - alarmUntil) >= 0) { stopAlarm(); return; }
  if (millis() - lastAlarmToggle > 200) {
    lastAlarmToggle = millis();
    alarmHigh = !alarmHigh;
    tone(BUZZER_PIN, alarmHigh ? 2500 : 1500);
    if (alarmHigh) setStrip(255, 0, 0); else setStrip(0, 0, 0);
    if (alarmBook >= 0) digitalWrite(books[alarmBook].ledPin, alarmHigh);
  }
}

void setArmed(bool a) {
  armed = a;
  if (!armed && alarmActive) stopAlarm();
  Serial.println(armed ? "Anti-theft ARMED (library closed)." : "Anti-theft DISARMED.");
  beep(armed ? 1800 : 900, 150);
  if (navMask == 0 && !alarmActive) setIdleOled(); else showOled();
}

void handleButton() {
  bool b = digitalRead(ARM_BTN_PIN);
  if (b != lastBtn && millis() - lastBtnChange > 200) {
    lastBtnChange = millis();
    lastBtn = b;
    if (b == LOW) setArmed(!armed);
  }
}

// ======================= Shelf sensors =======================
void onBookRemoved(int i) {
  books[i].picks++;
  Serial.printf("[SHELF] %s taken from %s\n", books[i].title, books[i].location);

  if (navMask & (1 << i)) {                 // user found the book they were guided to
    navMask &= ~(1 << i);
    digitalWrite(books[i].ledPin, LOW);
    Serial.printf("[NAV] %s picked up - navigation done.\n", books[i].title);
    beep(1500, 80);
  }

  if (armed) {
    startAlarm(i, 0);                       // runs until DISARM
    Serial.printf("[THEFT ALERT] %s removed while ARMED at %s!\n", books[i].title, books[i].location);
    oledLine1 = "!! THEFT ALERT !!";
    oledLine2 = books[i].title;
    oledLine3 = books[i].location;
    showOled();
  }
  printSync(i);
}

void onBookReturnedToShelf(int i) {
  Serial.printf("[SHELF] %s placed back at %s%s\n", books[i].title, books[i].location,
                books[i].issued ? "  (warning: still marked ISSUED - return it at the desk)" : "");
  beep(1500, 80);
  printSync(i);
}

void readSensors() {
  for (int i = 0; i < NUM_BOOKS; i++) {
    bool raw = (digitalRead(books[i].sensorPin) == LOW);   // LOW = book on shelf
    if (raw != books[i].lastRaw) {
      books[i].lastRaw = raw;
      books[i].lastChange = millis();
    }
    if ((millis() - books[i].lastChange) > DEBOUNCE_MS && raw != books[i].onShelf) {
      books[i].onShelf = raw;
      if (raw) onBookReturnedToShelf(i);
      else     onBookRemoved(i);
      if (!alarmActive && navMask == 0) setIdleOled();
      if (currentScreen == BOOK_DETAILS && detailsBook == i) drawBookDetails(i);
    }
  }
}

// ======================= RFID =======================
void issueBook(int idx, const String& uid) {
  books[idx].issued = true;
  books[idx].issues++;
  Serial.println("ISSUED," + uid);
  stripFlash(0, 255, 0, 1000);                            // green
  beep(1800, 150);
  drawResult(true, "BOOK ISSUED", books[idx].title, "Due back in 14 days", "Enjoy reading!");
  printSync(idx);
}

void returnBook(int idx, const String& uid) {
  books[idx].issued = false;
  Serial.println("RETURNED," + uid);
  stripFlash(0, 120, 255, 1000);                          // cyan
  beep(1800, 150);
  drawResult(true, "BOOK RETURNED", books[idx].title, String("Place it at ") + books[idx].location, "Thank you!");
  printSync(idx);
}

void unknownCard(const String& uid, const char* where) {
  Serial.println("UNKNOWN," + uid);
  stripFlash(255, 120, 0, 800);                           // orange
  beep(600, 300);
  drawResult(false, "UNKNOWN TAG", String("UID ") + uid, "This is not a", String("library book (") + where + ")");
}

void handleDesk() {
  if (!deskReader.PICC_IsNewCardPresent() || !deskReader.PICC_ReadCardSerial()) return;

  String uid = uidToString(deskReader);
  int idx = findBookByUid(deskReader);

  if (idx < 0) {
    unknownCard(uid, "desk");
  } else if (currentScreen == ISSUE_SCAN) {               // kiosk asked for ISSUE
    if (books[idx].issued) {
      Serial.println("ALREADY_ISSUED," + uid);
      beep(600, 300);
      drawResult(false, "NOT ISSUED", books[idx].title, "is already issued.", "Return it first.");
    } else issueBook(idx, uid);
  } else if (currentScreen == RETURN_SCAN) {              // kiosk asked for RETURN
    if (!books[idx].issued) {
      Serial.println("NOT_ISSUED," + uid);
      beep(600, 300);
      drawResult(false, "NOT RETURNED", books[idx].title, "was never issued.", "Nothing to return.");
    } else returnBook(idx, uid);
  } else {                                                // librarian quick mode: toggle
    if (books[idx].issued) returnBook(idx, uid);
    else                   issueBook(idx, uid);
  }
  deskReader.PICC_HaltA();
  deskReader.PCD_StopCrypto1();
}

void handleDoor() {
  if (!doorReader.PICC_IsNewCardPresent() || !doorReader.PICC_ReadCardSerial()) return;

  String uid = uidToString(doorReader);
  int idx = findBookByUid(doorReader);

  if (idx < 0) {
    unknownCard(uid, "door");
  } else if (books[idx].issued) {
    Serial.println("EXIT_OK," + uid);
    stripFlash(0, 255, 0, 1000);
    beep(1800, 150);
  } else {
    Serial.println("THEFT," + uid);
    Serial.printf("[THEFT ALERT] %s is leaving the library without being issued!\n", books[idx].title);
    startAlarm(idx, 2400);                                // same length as the old 6-cycle alarm
    oledLine1 = "!! DOOR ALERT !!";
    oledLine2 = books[idx].title;
    oledLine3 = "Not issued!";
    showOled();
    drawResult(false, "SECURITY ALERT", books[idx].title, "was not issued.", "Please visit the desk.");
  }
  doorReader.PICC_HaltA();
  doorReader.PCD_StopCrypto1();
}

// ======================= Serial commands =======================
void printSync(int i) {
  Serial.printf("SYNC {\"sku\":\"%s\",\"title\":\"%s\",\"category\":\"%s\",\"location\":\"%s\","
                "\"issued\":%s,\"onShelf\":%s,\"issues\":%d,\"picks\":%d,\"searches\":%d}\n",
                books[i].sku, books[i].title, books[i].category, books[i].location,
                books[i].issued ? "true" : "false", books[i].onShelf ? "true" : "false",
                books[i].issues, books[i].picks, books[i].searches);
}

void printList() {
  Serial.println("---- LIBRARY CATALOGUE ----");
  for (int i = 0; i < NUM_BOOKS; i++) {
    Serial.printf("%-14s %-10s %-5s %-17s %-10s card=%s\n",
                  books[i].title, books[i].category, books[i].sku,
                  books[i].location, statusText(i), books[i].card);
  }
}

void printReport() {
  Serial.println("---- ANALYTICS REPORT ----");
  int best = 0;
  for (int i = 0; i < NUM_BOOKS; i++) {
    Serial.printf("%-14s issues=%d searches=%d picks=%d %s\n",
                  books[i].title, books[i].issues, books[i].searches, books[i].picks, statusText(i));
    int score = books[i].issues + books[i].searches + books[i].picks;
    int bestScore = books[best].issues + books[best].searches + books[best].picks;
    if (score > bestScore) best = i;
  }
  Serial.printf("Most popular: %s\n", books[best].title);
  Serial.printf("Issued: %d   Off shelf: %d\n", countIssued(), countOffShelf());
  for (int i = 0; i < NUM_BOOKS; i++)
    if (!books[i].onShelf && !books[i].issued)
      Serial.printf("CHECK: %s is off the shelf but not issued (being read inside?)\n", books[i].title);
}

void printHelp() {
  Serial.println("Commands: HELP | LIST | SEARCH <text> | CLEAR | ARM | DISARM | REPORT | SYNC");
}

void handleSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;
  String up = line; up.toUpperCase();

  if (up == "HELP")                  printHelp();
  else if (up == "LIST")             printList();
  else if (up.startsWith("SEARCH"))  searchItems(line.substring(6));
  else if (up == "CLEAR")            { clearNav(); Serial.println("Navigation cleared."); }
  else if (up == "ARM")              setArmed(true);
  else if (up == "DISARM")           setArmed(false);
  else if (up == "REPORT")           printReport();
  else if (up == "SYNC")             { for (int i = 0; i < NUM_BOOKS; i++) printSync(i); }
  else                               Serial.println("Unknown command. Type HELP.");
}

// ======================= Kiosk drawing =======================
void drawHeader(const String& title, uint16_t color) {
  tft.fillRect(0, 0, 240, 45, color);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  int w = title.length() * 12;
  tft.setCursor((240 - w) / 2, 14);
  tft.print(title);
}

void drawButton(int x, int y, int w, int h, const String& text, uint16_t color) {
  tft.fillRoundRect(x, y, w, h, 8, color);
  tft.drawRoundRect(x, y, w, h, 8, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  int textWidth = text.length() * 12;
  tft.setCursor(x + (w - textWidth) / 2, y + (h - 16) / 2);
  tft.print(text);
}

void drawHomeButton(const char* label) {
  tft.fillRoundRect(20, 280, 200, 30, 5, ILI9341_DARKGREY);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  int w = strlen(label) * 6;
  tft.setCursor((240 - w) / 2, 291);
  tft.print(label);
}

void printAt(int x, int y, uint8_t size, uint16_t color, const String& s) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.setCursor(x, y);
  tft.print(s);
}

void drawHome() {
  currentScreen = HOME;
  tft.fillScreen(ILI9341_BLACK);
  drawHeader("FIND MY BOOK");
  drawButton(20, 60, 200, 50, "FIND BOOK");
  drawButton(20, 125, 200, 50, "ISSUE BOOK");
  drawButton(20, 190, 200, 50, "RETURN BOOK");
  drawButton(20, 255, 200, 50, "ABOUT US");
}

void drawFindBook() {
  currentScreen = FIND_BOOK;
  tft.fillScreen(ILI9341_BLACK);
  drawHeader("FIND BOOK");
  printAt(20, 52, 1, ILI9341_LIGHTGREY, "Tap a book to locate it on the shelf:");
  for (int i = 0; i < NUM_BOOKS; i++)
    drawButton(20, 68 + i * 52, 200, 44, books[i].title,
               books[i].issued ? ILI9341_MAROON : ILI9341_DARKCYAN);
  drawHomeButton();
}

void drawBookDetails(int i) {
  currentScreen = BOOK_DETAILS;
  detailsBook = i;
  tft.fillScreen(ILI9341_BLACK);
  drawHeader("BOOK DETAILS");
  printAt(20, 60, 2, ILI9341_YELLOW, books[i].title);
  printAt(20, 95,  1, ILI9341_WHITE, String("SKU:      ") + books[i].sku);
  printAt(20, 112, 1, ILI9341_WHITE, String("Category: ") + books[i].category);
  printAt(20, 129, 1, ILI9341_WHITE, String("Location: ") + books[i].location);

  uint16_t c = books[i].issued ? ILI9341_RED : (books[i].onShelf ? ILI9341_GREEN : ILI9341_ORANGE);
  printAt(20, 155, 2, c, statusText(i));

  if (books[i].issued) {
    printAt(20, 190, 1, ILI9341_LIGHTGREY, "This book is issued to a member.");
  } else {
    printAt(20, 190, 1, ILI9341_LIGHTGREY, "The LED on its shelf is blinking.");
    printAt(20, 207, 1, ILI9341_LIGHTGREY, "Take the book, then issue it");
    printAt(20, 224, 1, ILI9341_LIGHTGREY, "at the desk RFID reader.");
  }
  drawHomeButton();
}

void drawIssueBook() {
  currentScreen = ISSUE_BOOK;
  tft.fillScreen(ILI9341_BLACK);
  drawHeader("ISSUE BOOK");
  printAt(25, 80, 2, ILI9341_WHITE, "Scan your book");
  printAt(25, 115, 2, ILI9341_WHITE, "using RFID.");
  drawButton(20, 165, 200, 55, "START SCAN");
  drawHomeButton();
}

void drawReturnBook() {
  currentScreen = RETURN_BOOK;
  tft.fillScreen(ILI9341_BLACK);
  drawHeader("RETURN BOOK");
  printAt(25, 80, 2, ILI9341_WHITE, "Scan your book");
  printAt(25, 115, 2, ILI9341_WHITE, "using RFID.");
  drawButton(20, 165, 200, 55, "START SCAN");
  drawHomeButton();
}

void drawScanScreen(bool issue) {
  currentScreen = issue ? ISSUE_SCAN : RETURN_SCAN;
  scanStart = millis();
  lastCountdown = -1;
  tft.fillScreen(ILI9341_BLACK);
  drawHeader(issue ? "ISSUE BOOK" : "RETURN BOOK", ILI9341_NAVY);
  printAt(20, 70, 2, ILI9341_WHITE, "Tap your book");
  printAt(20, 95, 2, ILI9341_WHITE, "on the DESK");
  printAt(20, 120, 2, ILI9341_WHITE, "RFID reader...");
  tft.drawCircle(120, 200, 30, ILI9341_CYAN);
  tft.drawCircle(120, 200, 20, ILI9341_CYAN);
  tft.fillCircle(120, 200, 8, ILI9341_CYAN);
  drawHomeButton("CANCEL");
  stripFlash(0, 0, 80, SCAN_TIMEOUT_MS);
}

void updateScanCountdown() {
  if (currentScreen != ISSUE_SCAN && currentScreen != RETURN_SCAN) return;
  unsigned long el = millis() - scanStart;
  if (el > SCAN_TIMEOUT_MS) {
    stripIdle();
    drawResult(false, "TIMED OUT", "No book was scanned.", "Please try again.", "");
    return;
  }
  int left = (SCAN_TIMEOUT_MS - el) / 1000;
  if (left != lastCountdown) {
    lastCountdown = left;
    tft.fillRect(20, 245, 200, 20, ILI9341_BLACK);
    printAt(70, 248, 2, ILI9341_LIGHTGREY, String("Wait ") + left + "s");
  }
}

void drawAboutUs() {
  currentScreen = ABOUT_US;
  tft.fillScreen(ILI9341_BLACK);
  drawHeader("ABOUT US");
  printAt(20, 70,  1, ILI9341_WHITE, "FIND MY BOOK");
  printAt(20, 100, 1, ILI9341_WHITE, "AI & IoT Smart Library");
  printAt(20, 125, 1, ILI9341_WHITE, "Navigation & Inventory");
  printAt(20, 165, 1, ILI9341_WHITE, "Smart book searching,");
  printAt(20, 185, 1, ILI9341_WHITE, "RFID issuing/returning,");
  printAt(20, 205, 1, ILI9341_WHITE, "shelf navigation and");
  printAt(20, 225, 1, ILI9341_WHITE, "anti-theft detection.");
  drawHomeButton();
}

void drawResult(bool ok, const String& title, const String& l1, const String& l2, const String& l3) {
  currentScreen = ok ? SUCCESS : ERROR_SCREEN;
  resultShownAt = millis();
  tft.fillScreen(ILI9341_BLACK);
  drawHeader(title, ok ? ILI9341_DARKGREEN : ILI9341_RED);
  if (ok) {
    tft.fillCircle(120, 100, 30, ILI9341_DARKGREEN);
    tft.drawLine(105, 100, 115, 112, ILI9341_WHITE);
    tft.drawLine(115, 112, 137, 88, ILI9341_WHITE);
  } else {
    tft.fillCircle(120, 100, 30, ILI9341_RED);
    tft.drawLine(108, 88, 132, 112, ILI9341_WHITE);
    tft.drawLine(132, 88, 108, 112, ILI9341_WHITE);
  }
  printAt(20, 150, 2, ILI9341_YELLOW, l1);
  printAt(20, 185, 1, ILI9341_WHITE, l2);
  printAt(20, 202, 1, ILI9341_WHITE, l3);
  drawHomeButton();
}

// ======================= Touch =======================
bool inBox(int x, int y, int bx, int by, int bw, int bh) {
  return x >= bx && x <= bx + bw && y >= by && y <= by + bh;
}

void handleTouch(int x, int y) {
  switch (currentScreen) {
    case HOME:
      if      (inBox(x, y, 20, 60, 200, 50))  drawFindBook();
      else if (inBox(x, y, 20, 125, 200, 50)) drawIssueBook();
      else if (inBox(x, y, 20, 190, 200, 50)) drawReturnBook();
      else if (inBox(x, y, 20, 255, 200, 50)) drawAboutUs();
      return;

    case FIND_BOOK:
      for (int i = 0; i < NUM_BOOKS; i++) {
        if (inBox(x, y, 20, 68 + i * 52, 200, 44)) {
          Serial.printf("[KIOSK] Find: %s -> %s (%s)\n", books[i].title, books[i].location, statusText(i));
          if (!books[i].issued) startNavigation(i);
          drawBookDetails(i);
          return;
        }
      }
      break;

    case ISSUE_BOOK:
      if (inBox(x, y, 20, 165, 200, 55)) { drawScanScreen(true); return; }
      break;

    case RETURN_BOOK:
      if (inBox(x, y, 20, 165, 200, 55)) { drawScanScreen(false); return; }
      break;

    default:
      break;
  }

  // HOME / CANCEL bar at the bottom of every other screen
  if (y >= 280) {
    if (currentScreen == ISSUE_SCAN || currentScreen == RETURN_SCAN) stripIdle();
    drawHome();
  }
}

void pollTouch() {
  if (!touchOK) return;
  bool t = touch.touched();
  if (t && !wasTouched) {                 // act once per press
    TS_Point p = touch.getPoint();
    int x = TOUCH_FLIP_X ? map(p.x, 0, 240, 240, 0) : p.x;
    int y = TOUCH_FLIP_Y ? map(p.y, 0, 320, 320, 0) : p.y;
    if (PRINT_TOUCH) Serial.printf("Touch: %d,%d\n", x, y);
    handleTouch(x, y);
  }
  wasTouched = t;
}

// ======================= Setup / Loop =======================
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("=========================================");
  Serial.println(" FIND MY BOOK - SMART LIBRARY (ESP32-S3)");
  Serial.println("=========================================");

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ARM_BTN_PIN, INPUT_PULLUP);
  for (int i = 0; i < NUM_BOOKS; i++) {
    pinMode(books[i].sensorPin, INPUT_PULLUP);
    pinMode(books[i].ledPin, OUTPUT);
    digitalWrite(books[i].ledPin, LOW);
    books[i].onShelf = (digitalRead(books[i].sensorPin) == LOW);
    books[i].lastRaw = books[i].onShelf;
  }

  // SPI devices: TFT + two RFID readers
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  pinMode(SS_PIN_DESK, OUTPUT); digitalWrite(SS_PIN_DESK, HIGH);
  pinMode(SS_PIN_DOOR, OUTPUT); digitalWrite(SS_PIN_DOOR, HIGH);
  tft.begin();
  tft.setRotation(0);
  deskReader.PCD_Init();          // hard reset (RST pin) + init
  doorReader.PCD_Init();          // soft reset + init
  Serial.println("RFID desk + door readers ready.");

  // I2C devices: touch + OLED
  Wire.begin(I2C_SDA, I2C_SCL);
  touchOK = touch.begin(40);
  Serial.println(touchOK ? "Touch controller ready." : "Touch controller not detected.");
  oledOK = oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!oledOK) Serial.println("OLED not found!");

  strip.begin();
  strip.setBrightness(STRIP_BRIGHTNESS);
  stripIdle();

  printHelp();
  printList();
  setIdleOled();
  drawHome();
  beep(1000, 100);
  Serial.println("Library system ready.");
}

void loop() {
  handleSerial();
  handleButton();
  readSensors();
  handleDesk();
  handleDoor();
  pollTouch();
  updateNavLEDs();
  handleAlarm();
  updateScanCountdown();

  if ((currentScreen == SUCCESS || currentScreen == ERROR_SCREEN) &&
      millis() - resultShownAt > RESULT_SCREEN_MS && !alarmActive) {
    drawHome();
  }

  if (!alarmActive && stripRevertAt && (long)(millis() - stripRevertAt) >= 0) {
    stripRevertAt = 0;
    stripIdle();
  }

  if (millis() - lastOledRefresh > 2000 && navMask == 0 && !alarmActive) {
    lastOledRefresh = millis();
    setIdleOled();
  }
  delay(5);
}
