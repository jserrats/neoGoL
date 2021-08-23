#include <FastLED.h>
#include <CRC32.h>

FASTLED_USING_NAMESPACE

const uint8_t mWidth = 32;
const uint8_t mHeight = 32;

#define DATA_PIN    15
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS (mWidth * mHeight)
#define BRIGHTNESS          128


CRGB leds[NUM_LEDS];
uint8_t matrix [mWidth][mHeight];
uint8_t old_matrix [mWidth][mHeight];

uint32_t history [mWidth];
int history_pointer = 0;

uint32_t colour[] = {CRGB::Black, CRGB::Blue, CRGB::Teal, CRGB::DarkCyan, CRGB::Aqua, CRGB::Azure, CRGB::Aquamarine,};
uint8_t len_colour = sizeof(colour) / sizeof(colour[0]);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("==== GAME OF LIFE ====");

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalPixelString);
  FastLED.setBrightness(BRIGHTNESS);

  firstFrame();
  paintFrame();
}


void loop()
{
  delay(100);

  gameOfLife();
  if (checkStable()) {
    firstFrame();
  }
  paintFrame();

  // EVERY_N_SECONDS( 10 ) { randomFrame(); }

}

void gameOfLife() {
  //Serial.println("Creating life");
  //debugMatrix();
  for (int i = 0; i < mWidth; i++) {
    for (int j = 0; j < mHeight; j++) {
      int neighbours = countNeighbours(i, j);

      if (old_matrix[i][j] > 0) {
        //alive
        if (neighbours == 2 || neighbours == 3 ) {
          //survive
          matrix[i][j]++;
        } else {
          //kill
          matrix[i][j] = 0;
        }
      } else {
        //dead
        if (neighbours == 3) {
          //birth
          matrix[i][j] = 1;
        }
      }
    }
  }
}

uint32_t calcCRC() {
  CRC32 crc;
  for (int i = 0; i < mHeight; i++) {
    for (int j = 0; j < mWidth; j++) {
      crc.update(matrix[i][j] > 0);
    }
  }
  return crc.finalize();
}

bool checkStable() {
  uint32_t checksum = calcCRC();
  for (int i; i < mWidth; i++) {
    if (history[i] == checksum) {
      return true;
    }
  }

  history[history_pointer] = checksum;
  history_pointer = (history_pointer + 1) % mWidth;

  return false;
}

void randomFrame() {
  random16_add_entropy(random(10000));
  for (int i = 0; i < mHeight; i++) {
    for (int j = 0; j < mWidth; j++) {
      matrix[i][j] = random8(2); // random 0 or 1
    }
  }
}

void firstFrame() {
  //testColours();
  randomFrame();
}

void testColours() {
  for (int i = 0; i < len_colour; i++) {
    matrix[i][0] = i;
  }
}

int countNeighbours(int x, int y) {
  int neighbours = 0;

  for (int i = x - 1; i < x + 2; i++) {
    for (int j = y - 1; j < y + 2; j++) {
      int ii = (i + mWidth) % mWidth;
      int jj = (j + mHeight) % mHeight;
      if (old_matrix[ii][jj] > 0 && !(ii == x && jj == y)) {
        neighbours++;
      }
    }
  }
  return neighbours;
}

void debugMatrix() {
  Serial.println("Current state of matrix:");
  for (int i = mHeight - 1; i >= 0; i--) {
    for (int j = 0; j < mWidth; j++) {
      Serial.print(matrix[j][i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void swapMatrix() {
  for (int i = 0; i < mHeight; i++) {
    for (int j = 0; j < mWidth; j++) {
      old_matrix[i][j] = matrix[i][j];
    }
  }
}

void paintFrame() {
  //debugMatrix();

  for (int i; i < NUM_LEDS; i++) {
    int x, y;
    y = i / mHeight;

    if (y % 2 == 0) { //even row
      x = mWidth - ((i % mWidth) + 1) ;
    } else {
      x = i % mWidth;
    }
    
    int cycles = matrix[x][y];
    if (cycles >= len_colour ) {
      leds[i] = colour[len_colour];
    } else {
      leds[i] = colour[cycles];
    }
  }
  FastLED.show();
  swapMatrix();
}
