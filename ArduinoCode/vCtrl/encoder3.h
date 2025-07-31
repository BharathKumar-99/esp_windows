
#include <ezButton.h>  
#include "websocket.h"

#define E3_CLK_PIN 33  
#define E3_DT_PIN  32  
#define E3_SW_PIN  35  

#define E3_DIRECTION_CW  0    
#define E3_DIRECTION_CCW 1   

int E3_counter = 0;
int E3_direction = E3_DIRECTION_CW;
int E3_CLK_state;
int E3_prev_CLK_state;

ezButton E3_button(E3_SW_PIN);   

void init3(){
  pinMode(E3_CLK_PIN, INPUT);
  pinMode(E3_DT_PIN, INPUT);
  E3_button.setDebounceTime(50);  
 
  E3_prev_CLK_state = digitalRead(E3_CLK_PIN);
}

void encoder3Run( ){
  E3_button.loop();   

   
  E3_CLK_state = digitalRead(E3_CLK_PIN);
 
  if (E3_CLK_state != E3_prev_CLK_state && E3_CLK_state == HIGH) {
     
    if (digitalRead(E3_DT_PIN) == HIGH) {
      E3_counter--;
      E3_direction = E3_DIRECTION_CCW; 
      sendWebSocket("mic_down");
    } else {
      sendWebSocket("mic_up");
      E3_counter++;
      E3_direction = E3_DIRECTION_CW;
    }

    Serial.print("Rotary Encoder:: direction: ");
    if (E3_direction == E3_DIRECTION_CW)
      Serial.print("Clockwise");
    else
      Serial.print("Counter-clockwise");

    Serial.print(" - count: ");
    Serial.println(E3_counter);
  }

  
  E3_prev_CLK_state = E3_CLK_state;

  if (E3_button.isPressed()) {
    sendWebSocket("mic_toggle");
    Serial.println("The button is pressed");
  }
}