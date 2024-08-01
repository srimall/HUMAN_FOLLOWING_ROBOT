#define MAX_PWM 80
int incomingByte;
char forword[2] = "F";
char backword[2] = "B";
int  status_b = 0;
int pwm;
int pwn_f;

void setup()
{
  Serial.begin(9600);
  
  /* Right Motor */
  pinMode(9,OUTPUT);  //PWM
  pinMode(26,OUTPUT); //INA
  pinMode(28,OUTPUT); //INB
  
  
  /* Left Motor */
  pinMode(8,OUTPUT);    //PWM;
  pinMode(22,OUTPUT);   //INA
  pinMode(24,OUTPUT);   //INB
  
  
  analogWrite( 8, 0);
  analogWrite( 9, 0);
  
}


void move_backword()
{
  digitalWrite(26,HIGH);
  digitalWrite(28,LOW);
  
  digitalWrite(22,HIGH);
  digitalWrite(24,LOW);
  
  
  if( status_b == 0  )
  {
     for( pwm = 0; pwm < MAX_PWM; pwm = pwm + 10 )
     {
      analogWrite( 9, pwm );
      analogWrite( 8, pwm );
     }
     status_b = 1;
  }
  else
  {
      analogWrite( 9, MAX_PWM );
      analogWrite( 8, MAX_PWM);
  }
  
  
  
  
//  analogWrite( 9, 70 );
//  analogWrite( 8, 70 );

 
}

void move_forword()
{
  
  digitalWrite(28,HIGH);
  digitalWrite(26,LOW);
  
  digitalWrite(24,HIGH);
  digitalWrite(22,LOW);
  
  
  if( status_b == 0  )        // move_forword first time
  {
     for( pwm = 0; pwm < MAX_PWM; pwm = pwm + 10 )
     {
      analogWrite( 9, pwm );
      analogWrite( 8, pwm );
     }
     status_b = 1;                 
  }
  else                     // if already moving forword
  {
      analogWrite( 9, MAX_PWM );
      analogWrite( 8, MAX_PWM);
  }
  
  
  
//  analogWrite( 9, 70 );
//  analogWrite( 8, 70 );
}

void stop_motors()
{
  
  status_b = 0;
 
//  analogWrite( 9, 0 );
//  analogWrite( 8, 0);
//  delay(100);
//  digitalWrite(28,HIGH);
//  digitalWrite(26,HIGH);
//  
//  digitalWrite(24,HIGH);
//  digitalWrite(22,HIGH);

  for( int brake = pwm; brake <= 0 ; brake = brake - 10 )
  {
    analogWrite( 9, brake );
      analogWrite( 8, brake );
  }
  
  digitalWrite(28,HIGH);
  digitalWrite(26,HIGH); 
  
  digitalWrite(24,HIGH);
  digitalWrite(22,HIGH);

  
}

void loop()

{
  
  if( Serial.available() > 0 )
  {
     incomingByte  = Serial.read();
      if( incomingByte == 70 )
        {
          move_forword();
        }
      else if( incomingByte == 66)
       {
         move_backword();
       }
       else if(incomingByte == 83  )
         stop_motors();
  }
  
//  move_forword();
//  delay(1000);
//  stop_motors();
  // delay(100); // check direction informatio one in 100ms
   
  
}



