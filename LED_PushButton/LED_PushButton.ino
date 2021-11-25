#include <ESP8266WiFi.h>

int led = 5;     // LED pin
int button = 16; // push button is connected
int temp = 0;    // temporary variable for reading the button pin status

int ButtonState     = 0;   // take current button state
int LastButtonState = 0;   // take last button state
int LEDState        = 0;   // take light status

void setup() {
  pinMode(led, OUTPUT);   // declare LED as output
  pinMode(button, INPUT_PULLUP); // declare push button as input
}

void loop() {

ButtonState= digitalRead(button);
  
  if (LastButtonState == 0 && ButtonState == 1)
  {
    if (LEDState == 0)
    {
      digitalWrite(led, HIGH);
      LEDState = 1;
    }
    
    else
    {
      digitalWrite(led, LOW);
      LEDState = 0;
    }
  }
  LastButtonState = ButtonState;
}
