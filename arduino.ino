 
   
boolean motor_start =false;
#define pin_a  2   
//#define pin_b  3   
#define pin_fwd 4 
#define pin_bwd 5
#define pin_pwm 6  

int encoder=0;

float set_speed=0; // target speed
float ms_speed=0; // measuered speed
float pwm_pulse=0; //0-255
float error_speed=0; //set_speed - ms_speed
float error_speed_pre=0; //last error of speed
float error_speed_sum=0; //sum error of speed

float KP=0;  // proportional gain
float KI=0;  //integral gain
float KD=0; //derivative gain

int timer_counter;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_a,INPUT);                                                                                                                                                     
  //pinMode(pin_b,INPUT);
  pinMode(pin_fwd,OUTPUT);
  pinMode(pin_bwd,OUTPUT);
  pinMode(pin_pwm,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pin_a), detect_a, RISING);
  Serial.begin(9600); // Start serial port at 9600 bps.
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  timer_counter = 59286;   // preload timer 65536-16MHz/256/2Hz (34286 for 0.5sec) (59286 for 0.1sec)

  
  TCNT1 = timer_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

void loop() {
  
  digitalWrite(pin_fwd,1);   //run motor forward
  digitalWrite(pin_bwd,0);
  motor_start=true;
  
  if( Serial.available ()>0 ){
    
    set_speed=Serial.parseFloat();
    KP = Serial.parseFloat();
    KI = Serial.parseFloat();
   
    KD = Serial.parseFloat();
    
    Serial.println(ms_speed);
    delay(100);
  }  
  

}
void detect_a() {
  encoder+=1; //increasing encoder at new pulse
}

ISR(TIMER1_OVF_vect)        // interrupt  every 0.1sec
{
  TCNT1 = timer_counter;   // set timer
  ms_speed = 60.0*(encoder/100.0)/0.1;  //calculate motor speed, unit is rpm
  encoder=0;
 

if (motor_start){
    error_speed = set_speed - ms_speed; // calculate the error
    pwm_pulse = error_speed*KP + error_speed_sum*KI + (error_speed - error_speed_pre)*KD; // control signal
    error_speed_pre = error_speed;  //save previous error
    error_speed_sum += error_speed; //sum of error
  }
  else{ // if motor does not start
    error_speed = 0;
    error_speed_pre = 0;
    error_speed_sum = 0;
    pwm_pulse = 0;
  }
  if (pwm_pulse <255 & pwm_pulse >0){
    analogWrite(pin_pwm,pwm_pulse);  //set motor speed  
  }
  else{
    if (pwm_pulse>255){
      analogWrite(pin_pwm,255); 
    }
    else{
      analogWrite(pin_pwm,0);
    }
  }
  
}
