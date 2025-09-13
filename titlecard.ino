#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Dot matrix style parameters (smaller font than before)
const uint8_t DOT_SIZE = 2;      // size of each dot
const uint8_t DOT_SPACING = 1;   // spacing between dots
const uint8_t CHAR_SPACING = 4;  // space between characters
const uint16_t STEP_DELAY = 20;  // ms delay per dot

// Vertical spacing between "SUPER" and "STAR"
const uint8_t WORD_SPACING = 15; // Increased space between words

// 5x7 font bitmaps
typedef uint8_t CharBitmap[7];

const CharBitmap BM_S = {
  0b01110,
  0b10001,
  0b10000,
  0b01110,
  0b00001,
  0b10001,
  0b01110
};
const CharBitmap BM_U = {
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b01110
};
const CharBitmap BM_P = {
  0b11110,
  0b10001,
  0b10001,
  0b11110,
  0b10000,
  0b10000,
  0b10000
};
const CharBitmap BM_E = {
  0b11111,
  0b10000,
  0b10000,
  0b11110,
  0b10000,
  0b10000,
  0b11111
};
const CharBitmap BM_R = {
  0b11110,
  0b10001,
  0b10001,
  0b11110,
  0b10100,
  0b10010,
  0b10001
};
const CharBitmap BM_T = {
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};
const CharBitmap BM_A = {
  0b01110,
  0b10001,
  0b10001,
  0b11111,
  0b10001,
  0b10001,
  0b10001
};

// lookup function
const CharBitmap* getBitmap(char c) {
  switch (c) {
    case 'S': return &BM_S;
    case 'U': return &BM_U;
    case 'P': return &BM_P;
    case 'E': return &BM_E;
    case 'R': return &BM_R;
    case 'T': return &BM_T;
    case 'A': return &BM_A;
    case ' ': return nullptr;
    default:  return nullptr;
  }
}

// draw a single dot-matrix character with animation
void drawCharAnimated(char c, int startX, int startY) {
  const CharBitmap* b = getBitmap(c);
  if (!b) return;

  for (uint8_t row = 0; row < 7; row++) {
    uint8_t rowBits = (*b)[row];
    for (uint8_t col = 0; col < 5; col++) {
      bool bitOn = (rowBits & (1 << (4 - col))) != 0;
      if (bitOn) {
        int px = startX + col * (DOT_SIZE + DOT_SPACING);
        int py = startY + row * (DOT_SIZE + DOT_SPACING);
        display.fillRect(px, py, DOT_SIZE, DOT_SIZE, SSD1306_WHITE);
        display.display();
        delay(STEP_DELAY);
      }
    }
  }
}

// draw an entire word with animation
void drawDotMatrixWordAnimated(const char* word, int startX, int startY) {
  int x = startX;
  for (int i = 0; word[i] != '\0'; i++) {
    drawCharAnimated(word[i], x, startY);
    int charWidth = 5 * DOT_SIZE + 4 * DOT_SPACING;
    x += charWidth + CHAR_SPACING;
  }
}

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
  const char* topWord = "SUPER";
  const char* bottomWord = "STAR";

  int wordHeight = 7 * (DOT_SIZE + DOT_SPACING) + 1;

  // Calculate the total height of the entire block for vertical centering
  int totalHeight = (2 * wordHeight) + WORD_SPACING;
  int verticalOffset = (SCREEN_HEIGHT - totalHeight) / 2;

  // Top word position
  int topLen = strlen(topWord);
  int topTotalW = topLen * (5 * DOT_SIZE + 4 * DOT_SPACING) + (topLen - 1) * CHAR_SPACING;
  int topStartX = (SCREEN_WIDTH - topTotalW) / 2;
  int topStartY = verticalOffset;

  // Bottom word position
  int botLen = strlen(bottomWord);
  int botTotalW = botLen * (5 * DOT_SIZE + 4 * DOT_SPACING) + (botLen - 1) * CHAR_SPACING;
  int botStartX = (SCREEN_WIDTH - botTotalW) / 2;
  int botStartY = topStartY + wordHeight + WORD_SPACING;

  display.clearDisplay();
  display.display();

  // Draw SUPER and STAR with animation
  drawDotMatrixWordAnimated(topWord, topStartX, topStartY);
  display.display(); // Final display to ensure all dots are shown
  delay(200);

  drawDotMatrixWordAnimated(bottomWord, botStartX, botStartY);
  display.display(); // Final display to ensure all dots are shown

  delay(800);

  // --- Start of "RAJINI" superimposition logic ---
  const char* name = "RAJNI";
  
  // To get the text size for centering, we temporarily set the font size
  display.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(name, 0, 0, &x1, &y1, &w, &h);

  // Calculate the vertical midpoint between "SUPER" and "STAR"
  int rajiniY = topStartY + wordHeight + (WORD_SPACING / 2) - (h / 2);

  // Calculate the horizontal center for "RAJINI"
  int rajiniX = (SCREEN_WIDTH - w) / 2;

  // Flash "RAJINI" in place without a permanent background
  for (int i = 0; i < 6; i++) {
    if (i % 2 == 0) {
      // Draw "RAJINI"
      display.setTextColor(SSD1306_WHITE);
    } else {
      // Erase "RAJINI"
      display.setTextColor(SSD1306_BLACK);
    }
    display.setCursor(rajiniX, rajiniY);
    display.println(name);
    display.display();
    delay(400); // Flash delay
  }

  // Draw "RAJINI" one final time to make it stay on the screen without a background
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(rajiniX, rajiniY);
  display.println(name);
  display.display();

  delay(2000); // Pause before repeating the loop
}
