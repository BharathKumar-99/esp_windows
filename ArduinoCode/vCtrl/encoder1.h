
#include <ezButton.h>  

#define CLK_PIN 13  
#define DT_PIN  12  
#define SW_PIN  14  


#define DIRECTION_CW  0    
#define DIRECTION_CCW 1   

int counter = 0;
int direction = DIRECTION_CW;
int CLK_state;
int prev_CLK_state;

ezButton button(SW_PIN);   
ezButton pausePlay(22);

void init1(){
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  button.setDebounceTime(50);  
  pausePlay.setDebounceTime(50);  
 
  prev_CLK_state = digitalRead(CLK_PIN);
}

void encoder1Run(BleKeyboard &bleKeyboard){
  button.loop();   
  pausePlay.loop();   

   
  CLK_state = digitalRead(CLK_PIN);
 
  if (CLK_state != prev_CLK_state && CLK_state == HIGH) {
     
    if (digitalRead(DT_PIN) == HIGH) {
      counter--;
      direction = DIRECTION_CCW;
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    } else {
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      counter++;
      direction = DIRECTION_CW;
    }

    Serial.print("Rotary Encoder:: direction: ");
    if (direction == DIRECTION_CW)
      Serial.print("Clockwise");
    else
      Serial.print("Counter-clockwise");

    Serial.print(" - count: ");
    Serial.println(counter);
  }

  
  prev_CLK_state = CLK_state;

  if (button.isPressed()) {
    bleKeyboard.write(KEY_MEDIA_MUTE);
    Serial.println("The button is pressed");
  }
  if (pausePlay.isPressed()) {
     bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    Serial.println("The button is pressed");
  }
}