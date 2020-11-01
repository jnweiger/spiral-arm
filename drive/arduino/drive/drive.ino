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
 *
 * v1.0 2020-10-29, jw	- simple working reversal switch.
 * v1.1 2020-10-31, jw	- timebase 10msec, introduced struct motor and do_motor() instead of many global vars and duplicated code.
 * v1.2 2020-10-31, jw	- pwm_pattern instroduced. slow start after button press, ACCEL_TIME=10 is just fine.
 */


uint16_t led_cnt = 0;
#define LED_INTERVALL 50	// unit of 10 msec

#define SWITCH_PRESSED LOW	/* Switch is connected to GND. Default: HIGH due to pullup */
#define SWITCH_DIS_TIME 100	// unit of 10 msec
#define PAUSE_TIME 400		// unit of 10 msec. remain fully rolled up.
#define ACCEL_TIME 10		// unit of 80 msec. Softstart ramp.


typedef struct {
  uint8_t switch_pin;
  uint8_t bridge1_pin;
  uint8_t bridge2_pin;

  uint8_t accel_cnt;		// count down until next higher speed.
  uint8_t pwm_speed;		// which pwm array we use. 0 = 0% 8 = 100%
  uint8_t pwm_bit;		// which pwm bit position we use.
  uint8_t fwd;			// boolean
  uint16_t debounce_cnt;	// count down while deboucing reversal switch
  uint16_t pause_cnt; 		// count down while motor is pausing
} MOTOR;

MOTOR m1 = { PIN_PB1, PIN_PD2, PIN_PD3, ACCEL_TIME, 1 };
MOTOR m2 = { PIN_PB0, PIN_PD4, PIN_PD5, ACCEL_TIME, 1 };

#define N_PWM_PATTTERN 9
uint8_t pwm_pattern[N_PWM_PATTTERN][8] = 	// we only run at 100 Hz, so we better spread out the bits to achieve higher effective frequencies and less stutter.
{
  { 0,0,0,0,0,0,0,0 },	// 0/8   0%
  { 1,0,0,0,0,0,0,0 },  // 1/8  12%
  { 1,0,0,0,1,0,0,0 },  // 2/8  25%
  { 1,0,0,1,0,1,0,0 },	// 3/8  37%
  { 1,0,1,0,1,0,1,0 },  // 4/8  50%
  { 1,1,0,1,0,1,1,0 },	// 5/8  62%
  { 1,1,1,0,1,1,1,0 },	// 6/8  75%
  { 1,1,1,1,1,1,1,0 },	// 7/8  88%
  { 1,1,1,1,1,1,1,1 } 	// 8/8 100%
};

void do_motor(MOTOR *m)
{
  if (m->debounce_cnt == 0)
  {
    if (digitalRead(m->switch_pin) == SWITCH_PRESSED)
    {
      m->fwd = 1 - m->fwd;  /* reverse that motor */
      m->debounce_cnt = SWITCH_DIS_TIME;
      m->pause_cnt = PAUSE_TIME;
      m->pwm_speed = 2;			// prepare slow start after button press
      m->accel_cnt = ACCEL_TIME;	// prepare slow start after button press
    }
  }
  else
  {
    if (m->pause_cnt > 0)
      m->pause_cnt--;
    else
      m->debounce_cnt--;
  }

  uint8_t pulse = 0;
  if (m->pause_cnt == 0)
    {
      // advance pwm pattern and acceleration only when unpaused
      uint8_t bit = m->pwm_bit;
      pulse = pwm_pattern[m->pwm_speed][bit];
      if (++bit >= sizeof(pwm_pattern[0]))
        {
	  bit = 0;
          if (m->accel_cnt > 0)		// remain a bit longer at this speed?
            {
              m->accel_cnt--;
            }
          else if (m->pwm_speed < N_PWM_PATTTERN-1)	// can we switch to the next higher speed?
            {
	      m->pwm_speed++;
              m->accel_cnt = ACCEL_TIME;	// maintain that new speed for another acceleraton timer round.
            }
	  // if we are already at highest speed, we let accel_cnt drop to zero and it remains there.
        }
      m->pwm_bit = bit;	// regardless of speed and acceleration, the pwm_bit loops from 0 to 7 and back to 0.
    }

  if (pulse)
  {
    if (m->fwd)
    {
      digitalWrite(m->bridge1_pin, HIGH);
      digitalWrite(m->bridge2_pin, LOW);
    }
    else
    {
      digitalWrite(m->bridge1_pin, LOW);
      digitalWrite(m->bridge2_pin, HIGH);
    }
  }
  else // pwm pulse is low, or motor is paused.
  {
    digitalWrite(m->bridge1_pin, LOW);
    digitalWrite(m->bridge2_pin, LOW);
  }
}


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

  do_motor(&m1);
  do_motor(&m2);

  led_cnt++;
  if (led_cnt > LED_INTERVALL)
    led_cnt = 0;
  if (led_cnt == 0)
    digitalWrite(PIN_PB2, HIGH);
  else
    digitalWrite(PIN_PB2, LOW);

  delay(10);                       // wait 10 msec
}
