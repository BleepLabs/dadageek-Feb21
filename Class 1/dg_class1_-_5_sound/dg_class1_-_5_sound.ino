//blinking is just slow oscillation. Lets make it into something we can hear 

int led1_pin =  10;
int led1_state = 0;

int led2_pin =  9;
int led2_state = 0;

int audio_pin =  20;
int audio_state = 0;

unsigned long  previous_time1; //Initializing a variable without saying "=" defaults it to 0
unsigned long  previous_time2;
unsigned long  previous_time3;

unsigned long current_time;

unsigned long rate1;
unsigned long rate2;
unsigned long rate3;

int button_pin; 
int button_state; 

int pot_value;
int pot_pin = A0;

void setup() {
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(audio_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP);
  //analog inputs do not need to be setup
}

void loop()
{
  current_time = millis();

  button_state = digitalRead(button_pin); //if the button is not being pressed it will read 1. if it pressed it will read 0

  pot_value = analogRead(pot_pin); //Read the analog voltage at pin A0. Returns 0 for 0 Volts and 1023 for the max voltage (3.3V)
  rate1 = pot_value / 2;
  rate2 = pot_value / 5;
  rate3 = pot_value / 10;


  if (current_time - previous_time1 > rate1) {
    previous_time1 = current_time;

    if (led1_state == LOW) {
      led1_state = HIGH;
    }
    else {
      led1_state = LOW;
    }
    digitalWrite(led1_pin, led1_state); 
  }


  if (current_time - previous_time2 > rate2) {
    previous_time2 = current_time;

    if (led2_state == 0) {
      led2_state = 1;
    }
    else {
      led2_state = 0;
    }
    digitalWrite(led2_pin, led2_state); 
  }

  if (current_time - previous_time3 > rate3) {
    previous_time3 = current_time;

    if (audio_state == 0) {
      audio_state = 1;
    }
    else {
      audio_state = 0;
    }
    digitalWrite(audio_pin, audio_state); 
  }
  
}
