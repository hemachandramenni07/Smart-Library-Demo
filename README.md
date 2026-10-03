# FIND MY BOOK – Smart Library (combined project)

One ESP32-S3 now runs all three earlier circuits together:

| Old project | What it does now | Parts |
|---|---|---|
| touchscreen | Touch kiosk: Find / Issue / Return / About | ILI9341 with FT6206 capacitive touch |
| DE_casestudy | RFID issue/return at the desk, theft check at the door | 2 × MFRC522, WS2812 strip, buzzer |
| c1 | Shelf navigation, shelf sensors, night anti-theft, Serial commands | 4 slide switches, 4 LEDs, SSD1306 OLED, ARM button |

## Run it in VS Code (PlatformIO + Wokwi)

1. Install the **PlatformIO IDE** and **Wokwi Simulator** extensions in VS Code.
   The first time, press F1 → `Wokwi: Request a New License` and sign in (free).
2. Unzip this folder and open it with **File → Open Folder** (the folder that has `platformio.ini`).
3. Click the PlatformIO **Build** button (✓ in the bottom bar). The first build downloads the ESP32 tools and libraries, so it takes a few minutes.
4. Open `diagram.json` and press the green play button, or F1 → `Wokwi: Start Simulator`.

If you change the code, build again before restarting the simulator.

## How to demo it

**Find a book.** Tap *FIND BOOK* and pick a title. The kiosk shows its rack and shelf, the OLED says "Go to:", and that shelf's LED blinks. Slide that book's switch to the RIGHT (book taken off the shelf) and navigation stops.

**Issue / return.** Tap *ISSUE BOOK → START SCAN*. Click the **DESK** reader, choose a card and press **Tap**:

| Wokwi card | Book |
|---|---|
| Blue | Mathematics |
| Green | Physics |
| Yellow | History |
| Red | C Programming |
| Key Fob | not a library book (UNKNOWN) |

The strip flashes green and the kiosk shows "BOOK ISSUED". Returning works the same way through *RETURN BOOK*. If the kiosk is not waiting for a scan, the desk reader toggles issue/return by itself (librarian mode, as in the old project).

**Door anti-theft.** Tap a card on the **DOOR** reader. An issued book gives `EXIT_OK` (green). A book that was not issued sounds the alarm, flashes the strip red and shows "SECURITY ALERT".

**Night mode.** Press the red **ARM** button (or type `ARM`). Removing any book from its shelf now starts an alarm that runs until you press ARM again or type `DISARM`.

**Serial Monitor** (115200): `HELP`, `LIST`, `SEARCH <text>`, `CLEAR`, `ARM`, `DISARM`, `REPORT`, `SYNC`.

## Pin map (ESP32-S3-DevKitC-1)

| Part | Pins |
|---|---|
| SPI bus (TFT + both RFID) | SCK 12, MOSI 11, MISO 13 |
| ILI9341 TFT | CS 10, D/C 8, RST 9 |
| Desk RFID | SDA 14, RST 21 |
| Door RFID | SDA 47, RST 21 (shared) |
| I2C bus (touch 0x38 + OLED 0x3C) | SDA 6, SCL 7 |
| Shelf sensors A1, A2, B1, B2 | 4, 5, 15, 16 |
| Shelf LEDs A1, A2, B1, B2 | 17, 18, 38, 39 |
| Buzzer | 40 |
| ARM button | 41 |
| WS2812 strip (8 LEDs) | 42 |

Settings such as the scan timeout and touch-flip options are variables at the top of `src/main.cpp`.

## Using wokwi.com instead

Start a new **ESP32-S3** project, paste `src/main.cpp` into `sketch.ino`, paste `diagram.json`, and add a `libraries.txt` tab with the contents of `libraries.txt`.
