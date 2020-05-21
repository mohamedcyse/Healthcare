#include <Servo.h> //for importing the servo

Servo myservo;  // creating a servo object to control the servo

int pos = 5;    // variable to store the servo position

//---ULTRA SONIC GLOBALS---
const int trigPin_1 = 24;
const int echoPin_1 = 28;
long duration_1;
int distance_1;
const int trigPin_2 = 42;
const int echoPin_2 = 46;
long duration_2;
int distance_2;
const int trigPin_3 = 26;
const int echoPin_3 = 30;
long duration_3;
int distance_3;
const int trigPin_4 = 44;
const int echoPin_4 = 48;
long duration_4;
int distance_4;
int distance=0;
//---END ULTRA SONIC GLOBALS

//---TEMP GLOBALS---
int ThermistorPin = 0;
int Vo;
float R1 = 400;
float logR2, R2, T, Tc, Tf;
float c1 = 230.82, c2 = -26.71;
//---END TEMP GLOBALS---

//---INPUTS---
String US,Pu,Pre,Ml,MR,Temp;
//---END INPUTS---


//---PULSE GLOBALS---
int sensor_pin = A1;                

int led_pin = 13;                  

volatile int heart_rate;          

volatile int analog_data;              

volatile int time_between_beats = 600;            

volatile boolean pulse_signal = false;    

volatile int beat[10];         //heartbeat values will be sotred in this array    

volatile int peak_value = 512;          

volatile int trough_value = 512;    

volatile int thresh = 525;              

volatile int amplitude = 100;                 

volatile boolean first_heartpulse = true;      

volatile boolean second_heartpulse = false;    

volatile unsigned long samplecounter = 0;   //This counter will tell us the pulse timing

volatile unsigned long lastBeatTime = 0;
//---END PULSE GLOBALS---



void setup() {


  Serial.begin(9600);   //begin serial communication with speed (9600)<--can be changed.
  
  myservo.attach(8);  //attaches the servo on pin 8 to the servo object
  myservo.write(5); //sets servo degree to 5 degrees
  
  //---PULSE SETUP---
  pinMode(led_pin,OUTPUT);        
  interruptSetup();  
  //---END PULSE SETUP---           
  
  pinMode(trigPin_1, OUTPUT); // Sets the trigPin as an Output for UltraSonic1
  pinMode(echoPin_1, INPUT);  // Sets the echoPin as an Input for UltraSonic1

}

void loop() {
  readUS();
}

//---Ultra Sonic block---
void readUS(){
// Clears the trigPin
digitalWrite(trigPin_3, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin_3, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin_3, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration_3 = pulseIn(echoPin_3, HIGH);

// Calculating the distance
distance_3= duration_3*0.034/2;

//---End of ultraSonic block---

if(distance_3<=15){
//Serial.print("***WARNING!!! OBJECT IS GETTING CLOSER!!!***\n");
}

float avgTemp; //final result of Temp
int avgPulse; //final result of Pulse

String x=Serial.readString(); //start reading from Temp sensor

if(x=="up"){
      Serial.print("Arm is moving up..\n");
      myservo.write(pos=100);
  }

if(x=="temp"){
  //Serial.print("Measuring temperature..\n");
  for(int j=10;j>0;j--){
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  Tc = (c1 + c2*logR2 );

  avgTemp+=Tc;

    delay(1000);
  }
  
  Serial.print("Temperature: "); 
  Serial.print((avgTemp/10)+3);
  Serial.println(" C");  

   
}

if(x=="down"){
        Serial.print("Arm is moving down..\n");
        myservo.write(pos=5);
}
  
if (x=="pulse")
{ 
//  Serial.print("Measuring pulse..\n");
  for(int i=10;i>0;i--){

 avgPulse+=heart_rate;

      delay(1000); //  take a break
  }

        Serial.print("Blood/Minute: ");

      Serial.println(avgPulse/10);

}
}


void interruptSetup()

{    

  TCCR2A = 0x02;  // This will disable the PWM on pin 3 and 11

  OCR2A = 0X7C;   // This will set the top of count to 124 for the 500Hz sample rate

  TCCR2B = 0x06;  // DON'T FORCE COMPARE, 256 PRESCALER

  TIMSK2 = 0x02;  // This will enable interrupt on match between OCR2A and Timer

  sei();          // This will make sure that the global interrupts are enable

}


ISR(TIMER2_COMPA_vect)

{ 

  cli();                                     

  analog_data = analogRead(sensor_pin);            

  samplecounter += 2;                        

  int N = samplecounter - lastBeatTime;      


  if(analog_data < thresh && N > (time_between_beats/5)*3)

    {     

      if (analog_data < trough_value)

      {                       

        trough_value = analog_data;

      }

    }


  if(analog_data > thresh && analog_data > peak_value)

    {        

      peak_value = analog_data;

    }                          



   if (N > 250)

  {                            

    if ( (analog_data > thresh) && (pulse_signal == false) && (N > (time_between_beats/5)*3) )

      {       

        pulse_signal = true;          

        digitalWrite(led_pin,HIGH);

        time_between_beats = samplecounter - lastBeatTime;

        lastBeatTime = samplecounter;     



       if(second_heartpulse)

        {                        

          second_heartpulse = false;   

          for(int i=0; i<=9; i++)    

          {            

            beat[i] = time_between_beats; //Filling the array with the heart beat values                    

          }

        }


        if(first_heartpulse)

        {                        

          first_heartpulse = false;

          second_heartpulse = true;

          sei();            

          return;           

        }  


      word runningTotal = 0;  


      for(int i=0; i<=8; i++)

        {               

          beat[i] = beat[i+1];

          runningTotal += beat[i];

        }


      beat[9] = time_between_beats;             

      runningTotal += beat[9];   

      runningTotal /= 10;        

      heart_rate = 60000/runningTotal;

    }                      

  }




  if (analog_data < thresh && pulse_signal == true)

    {  

      digitalWrite(led_pin,LOW); 

      pulse_signal = false;             

      amplitude = peak_value - trough_value;

      thresh = amplitude/2 + trough_value; 

      peak_value = thresh;           

      trough_value = thresh;

    }


  if (N > 2500)

    {                          

      thresh = 512;                     

      peak_value = 512;                 

      trough_value = 512;               

      lastBeatTime = samplecounter;     

      first_heartpulse = true;                 

      second_heartpulse = false;               

    }


  sei();                                

}

