#ifndef ENCODER2_H
#define ENCODER2_H

#include <ezButton.h>  
#include "websocket.h"

// --- Pin Definitions ---
#define E2_CLK_PIN 27  
#define E2_DT_PIN  26  
#define E2_SW_PIN  25  

// --- Direction Flags ---
#define E2_DIRECTION_CW  0    
#define E2_DIRECTION_CCW 1   

// --- Globals ---
int E2_counter = 0;
int E2_direction = E2_DIRECTION_CW;
int E2_CLK_state;
int E2_prev_CLK_state;
bool encoder2Muted = false;  // toggle state

ezButton E2_button(E2_SW_PIN);  // push button

// --- Init Function ---
void init2() {
  pinMode(E2_CLK_PIN, INPUT);
  pinMode(E2_DT_PIN, INPUT);
  E2_button.setDebounceTime(50);  
  E2_prev_CLK_state = digitalRead(E2_CLK_PIN);
}

// --- Loop Function ---
void encoder2Run() {
  E2_button.loop();   
  E2_CLK_state = digitalRead(E2_CLK_PIN);

  // Handle rotation
  if (E2_CLK_state != E2_prev_CLK_state && E2_CLK_state == HIGH) {
    if (digitalRead(E2_DT_PIN) == HIGH) {
      E2_counter--;
      E2_direction = E2_DIRECTION_CCW;
      sendWebSocket("chrome_vol_down"); 
    } else {
      E2_counter++;
      E2_direction = E2_DIRECTION_CW;
      sendWebSocket("chrome_vol_up"); 
    }

    Serial.print("Encoder2 - Direction: ");
    Serial.print(E2_direction == E2_DIRECTION_CW ? "Clockwise" : "Counter-clockwise");
    Serial.print(" | Count: ");
    Serial.println(E2_counter);
  }

  E2_prev_CLK_state = E2_CLK_state;

  // Handle button press to toggle mute/unmute
  if (E2_button.isPressed()) {
    if (encoder2Muted) {
      sendWebSocket("encoder2_release");
      Serial.println("Encoder 2 Released (Unmute Chrome)");
    } else {
      sendWebSocket("encoder2_press");
      Serial.println("Encoder 2 Pressed (Mute Chrome)");
    }
    encoder2Muted = !encoder2Muted; // toggle state
  }
}

#endif
