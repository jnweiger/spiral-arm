/*
 * Motor reverse switch
 * Control one or two motors via L298N drivers.
 * 
 * When switch 1 is pressed, motor 1 pauses PAUSE_TIME, then reverses its direction.
 * Same for switch 2 and motor 2.
 * The switches are disabled (ignored) during the pause and for SWITCH_DIS_TIME afterwards for debouncing.
 * 
 * 
 * Switch 1: PIN_PB1   13
 * Switch 2: PIN_PB0   12
 * 
 * Motor 1: PIN_PD2 + PIN_PD3   6,7
 * Motor 2: PIN_PD4 + PIN_PD5   8,9
 * 
 * Flash LED: PIN_PB2   14
 */

uint8_t m1_fwd = 1;
uint8_t m2_fwd = 1;
uint8_t s1_cnt = 0;
uint8_t s2_cnt = 0;

uint8_t m1_pause = 0;
uint8_t m2_pause = 0;

uint8_t led_cnt = 0;

#define PAUSE_TIME 40
#define SWITCH_DIS_TIME 20
#define SWITCH_PRESSED LOW	/* Switch is connected to GND. Default: HIGH due to pullup */
#define LED_INTERVALL 5

void setup() {
  pinMode(PIN_PD2, OUTPUT);
  pinMode(PIN_PD3, OUTPUT);
  pinMode(PIN_PD4, OUTPUT);
  pinMode(PIN_PD5, OUTPUT);

  pinMode(PIN_PB2, OUTPUT);

  pinMode(PIN_PB0, INPUT_PULLUP);
  pinMode(PIN_PB1, INPUT_PULLUP);
}

void loop() {

  if (s1_cnt == 0)
  {
    if (digitalRead(PIN_PB1) == SWITCH_PRESSED)
    {
      m1_fwd = 1 - m1_fwd;  /* reverse that motor */
      s1_cnt = SWITCH_DIS_TIME;
      m1_pause = PAUSE_TIME;
    }
  }
  else
  {
    /* count down s1_cnt only when the motor moves */
    if (m1_pause > 0)
      m1_pause--;
    else
      s1_cnt--;
  }

  if (m1_pause)
  {
    digitalWrite(PIN_PD2, LOW);
    digitalWrite(PIN_PD3, LOW);
  }
  else
  {
    if (m1_fwd)
    {
      digitalWrite(PIN_PD2, HIGH);
      digitalWrite(PIN_PD3, LOW);
    }
    else
    {
      digitalWrite(PIN_PD2, LOW);
      digitalWrite(PIN_PD3, HIGH);
    }
  }
  
  
  if (s2_cnt == 0)
  {
    if (digitalRead(PIN_PB0) == SWITCH_PRESSED)
    {
      m2_fwd = 1 - m2_fwd;  /* reverse that motor */
      s2_cnt = SWITCH_DIS_TIME;
      m2_pause = PAUSE_TIME;
    }
  }
  else
  {
    if (m2_pause > 0)
      m2_pause--;
    else
      s2_cnt--;
  }
  
  if (m2_pause)
  {
    digitalWrite(PIN_PD4, LOW);
    digitalWrite(PIN_PD5, LOW);
  }
  else
  {
    if (m2_fwd)
    {
      digitalWrite(PIN_PD4, HIGH);
      digitalWrite(PIN_PD5, LOW);
    }
    else
    {
      digitalWrite(PIN_PD5, LOW);
      digitalWrite(PIN_PD4, HIGH);
    }
  }
  

  led_cnt++;
  if (led_cnt > LED_INTERVALL)
    led_cnt = 0;
  if (led_cnt == 0) 
    digitalWrite(PIN_PB2, HIGH);
  else
    digitalWrite(PIN_PB2, LOW);


  delay(100);                       // wait 0.1s
}
