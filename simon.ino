#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

//TODO: Sounds einbauen // Fix longer button press

ArduinoLEDMatrix matrix;
int score = 0;
char sequence[999];
char button_pressed = '\0';
char colors[] = {'r', 'g', 'b', 'y'};

const unsigned long debounceDelay = 50; // Debounce time in milliseconds

void setup() {
  
  Serial.begin(115200);
  //Pin 0 should not be connected
  randomSeed(analogRead(0));

  pinMode(4, OUTPUT); //y
  pinMode(5, OUTPUT); //r
  pinMode(6, OUTPUT); //g
  pinMode(7, OUTPUT); //b
  
  pinMode(9, INPUT); //y
  pinMode(10, INPUT); //r
  pinMode(11, INPUT); //g
  pinMode(12, INPUT); //b

  // Text when plugged in
  matrix.begin(); // LED Matrix init only neeeded in Setup Loop
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  const char text[] = "ROB";
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText();
  matrix.endDraw();

  delay(2500);

  draw_score();

  // Wait for button press to start game
  while (digitalRead(9) == LOW && digitalRead(10) == LOW && digitalRead(11) == LOW && digitalRead(12) == LOW) {}
  
  //___ADD STARTUP MELODY___
  delay(2000);

}


void loop() {

  append_random_char();

  //Make LEDs light up in sequence
  for (int i = 0; i < score + 1; i++) {

    switch (sequence[i]) {
      case 'y':
          digitalWrite(4, HIGH);
          delay(500);
          digitalWrite(4, LOW);
          break;
      case 'r':
          digitalWrite(5, HIGH);
          delay(500);
          digitalWrite(5, LOW);
          break;
      case 'g':
          digitalWrite(6, HIGH);
          delay(500);
          digitalWrite(6, LOW);
          break;
      case 'b':
          digitalWrite(7, HIGH);
          delay(500);
          digitalWrite(7, LOW);
          break;
    }
    //Delay not needed for last light
    if (i < score) {
      delay(500);
    }

  }

  // Take inputs and compare: If faulty, show final score
  for (int j = 0; j < score + 1; j++) {

    button_pressed = '\0';
    button_pressed = waitforButtonPress();

    if (button_pressed != sequence[j]) {
      //__ADD LOSE SOUND___
      digitalWrite(4, HIGH); digitalWrite(5, HIGH); digitalWrite(6, HIGH); digitalWrite(7, HIGH);
      delay(2500);
      digitalWrite(4, LOW); digitalWrite(5, LOW); digitalWrite(6, LOW); digitalWrite(7, LOW);
      delay(10);

      while (true) {
        draw_final_score();
      }
    }
      
  }

  score = score + 1;
  draw_score();
  delay(1000);

}



//FUNCTIONS

void append_random_char() {

  int randomIndex = random(4);
  char randomColor = colors[randomIndex];
  sequence[score] = randomColor;

  //Print the random character in Serial Monitor
  Serial.println(sequence);

}

//Debounce input
char waitforButtonPress() {
    char buttonChars[4] = {'y', 'r', 'g', 'b'};
    int buttonPins[4] = {9, 10, 11, 12};
    int buttonStates[4] = {LOW, LOW, LOW, LOW};
    int lastButtonStates[4] = {LOW, LOW, LOW, LOW};
    unsigned long lastDebounceTimes[4] = {0, 0, 0, 0};

    while (true) {
        for (int i = 0; i < 4; i++) {
            int currentButtonState = digitalRead(buttonPins[i]);

            if (currentButtonState != lastButtonStates[i]) {
                lastDebounceTimes[i] = millis(); // Reset debounce timer
            }

            if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
                if (currentButtonState != buttonStates[i]) {
                  buttonStates[i] = currentButtonState;
                  if (buttonStates[i] == LOW) {
                    //delay(100); // Prevent accidental multiple readings (Optional)
                    return buttonChars[i];
                    }
                }
            }

            lastButtonStates[i] = currentButtonState; // Save the button state for next iteration
        }
    }
}




void draw_score() {

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);

  char text[4];
  snprintf(text, sizeof(text), "%03d", score);

  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText();
  matrix.endDraw();

}


void draw_final_score() {

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(100);

  char final_text[8];
  snprintf(final_text, sizeof(final_text), "  %03d  ", score);

  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(final_text);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();

}