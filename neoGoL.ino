#include <FastLED.h>

FASTLED_USING_NAMESPACE

const uint8_t mWidth = 32;
const uint8_t mHeight = 32;

#define DATA_PIN    15
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS (mWidth * mHeight)
#define BRIGHTNESS          32
#define FRAMES_PER_SECOND  120


CRGB leds[NUM_LEDS];
int matrix [mWidth][mHeight];
int old_matrix [mWidth][mHeight];
int very_old_matrix [mWidth][mHeight];

uint32_t colour[] = {CRGB::DarkBlue, CRGB::Blue, CRGB::Aqua, CRGB::Aquamarine, CRGB::Azure,CRGB::Lavender};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("==== GAME OF LIFE ====");

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  randomFrame();
  paintFrame();
  //debugMatrix();

}


void loop()
{
  //debugMatrix();

  //delay(1000);
  gameOfLife();
  //debugMatrix();
  if (checkStable()) {
    randomFrame();
  }
  paintFrame();

  //debugMatrix();
  // do some periodic updates
  // EVERY_N_SECONDS( 10 ) { randomFrame(); }

}

void gameOfLife() {
  //Serial.println("Creating life");
  /*
  for (int i = 0; i < mHeight; i++) {
    for (int j = 0; j < mWidth; j++) {
      matrix[i][j] = 0;
    }
  }*/
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
          matrix[i][j]=1;
        }
      }

    }
  }
}

bool checkStable() {
  bool stable = true;
  for (int i = 0; i < mHeight; i++) {
    for (int j = 0; j < mWidth; j++) {
      if ((very_old_matrix[i][j]>0) != (matrix[i][j]>0)) {
        stable = false;
        break;
      }
    }
  }
  return stable;
}

void randomFrame() {
  //Serial.println("Drawing a random frame");
  random16_add_entropy(random(10000));
  for (int i = 0; i < mHeight; i++) {
    for (int j = 0; j < mWidth; j++) {
      matrix[i][j] = random8(2); // random 0 or 1
    }
  }
  //Serial.println("Finished drawing a random frame");
}

int countNeighbours(int x, int y) {
  /*Serial.println();
    Serial.print("[+] Counting neighbours for: ");
    Serial.print(x);
    Serial.print(" ");
    Serial.println(y);
  */int neighbours = 0;

  for (int i = x - 1; i < x + 2; i++) {
    for (int j = y - 1; j < y + 2; j++) {
      /*
        Serial.print("i:");
        Serial.println(i);
        Serial.print("j:");
        Serial.println(j);
      */
      int ii = (i + mWidth) % mWidth;
      int jj = (j + mHeight) % mHeight;
      /* Serial.print("Value of neighbour XY: ");
        Serial.print(i);
        Serial.print(":");
        Serial.print(j);
        Serial.print(" ");
        Serial.println(old_matrix[i][j]);
      */if (old_matrix[ii][jj] > 0 && !(ii == x && jj == y)) {
        neighbours++;
      }
    }
    //Serial.println();
  }
  /*
    Serial.print("[+] Pixel XY: ");
    Serial.print(x);
    Serial.print(" ");
    Serial.print(y);
    Serial.print(" has these neighbours: ");
    Serial.println(neighbours);
  */return neighbours;
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

void debugOldMatrix() {
  //Serial.println("Current state of OLD matrix:");
  for (int i = mHeight - 1; i >= 0; i--)  {
    for (int j = 0; j < mWidth; j++) {
      //Serial.print(old_matrix[j][i]);
      //Serial.print(" ");
    }
    //Serial.println();
  }
}

void swapMatrix() {
  for (int i = 0; i < mHeight; i++) {
    for (int j = 0; j < mWidth; j++) {
      very_old_matrix[i][j] = old_matrix[i][j];
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
    if (cycles == 0) {
      leds[i] = CRGB::Black;
    } else if (cycles > sizeof(colour)/sizeof(colour[0])) {
      leds[i] = CRGB::White;
    } else {
      leds[i] = colour[cycles-1];
    }
  }
  //Serial.println("Showing new frame");
  FastLED.show();
  //Serial.println("Swapping old and new frames");
  swapMatrix();
}
