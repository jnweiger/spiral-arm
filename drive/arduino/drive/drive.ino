/*
 * Motor reverse switch
 * Control one or two motors via L298N drivers.
 *
 * When switch 1 is pressed, motor 1 pauses PAUSE_TIME, then reverses its direction.
 * Same for switch 2 and motor 2.
 * The switches are disabled (ignored) during the pause and for SWITCH_DEBOUNCE_TIME afterwards for debouncing.
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
 * v1.3 2020-11-1, jw	- added midway fun with do_wink()
 */


uint16_t led_cnt = 0;
#define LED_INTERVALL 50	// unit of 10 msec

#define SWITCH_PRESSED LOW		/* Switch is connected to GND. Default: HIGH due to pullup */
#define SWITCH_DEBOUNCE_TIME 100	// unit of 10 msec
#define PAUSE_TIME 400			// unit of 10 msec. remain fully rolled up.
#define WINK_TIME_OFFSET 400		// unit of 10 msec. Avoid hitting "exactly the unrolled point, where the string is loose"
#define ACCEL_TIME 10			// unit of 80 msec. Softstart ramp.


typedef struct {
  uint8_t switch_pin;
  uint8_t bridge1_pin;
  uint8_t bridge2_pin;

  uint8_t accel_cnt;		// count down until next higher speed.
  uint8_t pwm_speed;		// which pwm_pattern array we use. 0 = 25%, 6 = 100%
  uint8_t pwm_bit;		// which pwm bit position we use.
  uint8_t fwd;			// boolean
  uint16_t debounce_cnt;	// count down while deboucing reversal switch
  uint16_t pause_cnt; 		// count down while motor is pausing
  uint16_t loop_time_interval; 	// how many loops we had between the last two button presses
  uint16_t time_cnt; 		// count up since the last button press.
} MOTOR;

MOTOR m1 = { PIN_PB1, PIN_PD2, PIN_PD3, ACCEL_TIME };
MOTOR m2 = { PIN_PB0, PIN_PD4, PIN_PD5, ACCEL_TIME };

#define N_PWM_PATTTERN 7
uint8_t pwm_pattern[N_PWM_PATTTERN][8] = 	// we only run at 100 Hz, so we better spread out the bits to achieve higher effective frequencies and less stutter.
{
  { 1,0,0,0,1,0,0,0 },  // 0:	2/8 = 25%
  { 1,0,0,1,0,1,0,0 },	// 1:	3/8 = 37%
  { 1,0,1,0,1,0,1,0 },  // 2:	4/8 = 50%
  { 1,1,0,1,0,1,1,0 },	// 3:	5/8 = 62%
  { 1,1,1,0,1,1,1,0 },	// 4:	6/8 = 75%
  { 1,1,1,1,1,1,1,0 },	// 5:	7/8 = 88%
  { 1,1,1,1,1,1,1,1 } 	// 6:	8/8 =100%
};


void do_wink(MOTOR *m)
{
  /*
   * This is a fun function.
   * We try to find the midpoint of the movement, when the arm is fully extended and starting to roll backwards.
   * Then we examine a random number (the last 3 bits of the loop_time_interval)
   * and based on that, we manipulate the motor, so that goes backward forward a few times.
   * The idea is that the little finger at the tip of the arm then does some inviting guesture.
   * In 5 out of 8 cases, we do nothing. Else we wink once, twice or 3 times.
   */
  uint16_t timer = m->time_cnt;
  if (m->loop_time_interval != 0 && timer > PAUSE_TIME+PAUSE_TIME+SWITCH_DEBOUNCE_TIME)	// do nothing, when we don't know the loop time, or when just starting a loop
    {
      uint16_t midpoint = WINK_TIME_OFFSET + (m->loop_time_interval) >> 1;
      uint8_t action    = m->loop_time_interval & 0x7;

      if (action < 3)
	{
	  if (		timer == midpoint)
            {
							m->accel_cnt = 60;	// no not auto-accellerate while we play here.
							m->pwm_speed = 2; 	// when at midpoint, we slow down.
	    }
	  if (		timer == midpoint + 100)	m->fwd = 1 - m->fwd;	// reverse motor
	  if (		timer == midpoint + 200)	m->fwd = 1 - m->fwd;	// a second later, we reverse again.
	  if (action < 2)							// do a second and maybe even a third wink?
	    {
	      if (	timer == midpoint + 300)	m->fwd = 1 - m->fwd;	// reverse again
	      if (	timer == midpoint + 400)	m->fwd = 1 - m->fwd;	// and again
	      if (action < 1)							// do a third wink?
		{
		  if (	timer == midpoint + 500)	m->fwd = 1 - m->fwd;	// reverse again
		  if (	timer == midpoint + 600)	m->fwd = 1 - m->fwd;	// and again
		}
              else if (	timer == midpoint + 500)	m->pwm_speed = 6; 	// back to full speed after only one wink
	    }
          else if (	timer == midpoint + 300)	m->pwm_speed = 6; 	// back to full speed after two winks
          if (		timer == midpoint + 700)	m->pwm_speed = 6; 	// back to full speed in any case.
	}
    }
}


void do_motor(MOTOR *m)
{
  if (m->debounce_cnt == 0)
  {
    if (digitalRead(m->switch_pin) == SWITCH_PRESSED)
    {
      m->fwd = 1 - m->fwd;  /* reverse that motor */
      m->debounce_cnt = SWITCH_DEBOUNCE_TIME;
      m->pause_cnt = PAUSE_TIME;
      m->pwm_speed = 0;			// prepare slow start after button press
      m->accel_cnt = ACCEL_TIME;	// prepare slow start after button press

      m->loop_time_interval = m->time_cnt;	// store frome the last round.
      m->time_cnt = 0;
    }
  }
  else
  {
    if (m->pause_cnt > 0)
      m->pause_cnt--;
    else
      m->debounce_cnt--;
  }
  m->time_cnt++;

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

  do_wink(&m1);
  do_wink(&m2);

  led_cnt++;
  if (led_cnt > LED_INTERVALL)
    led_cnt = 0;
  if (led_cnt == 0)
    digitalWrite(PIN_PB2, HIGH);
  else
    digitalWrite(PIN_PB2, LOW);

  delay(10);                       // wait 10 msec
}
