#define MQ3pin 0
#include<SoftwareSerial.h>
float sensorValue, sensorPer;  //variable to store sensor value
long duration;
int distance;

#define IN1 3
#define IN2 5
#define IN3 10
#define IN4 11
#define EN1 6
#define EN2 9
int vib_pin=12;
const int trigPin = 7;
const int echoPin = 8;


//efine speed

SoftwareSerial bt(0, 1); // RX, TX

char data;

void setup() { 

  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(EN1,220);
  analogWrite(EN2,220);
  bt.begin(9600);
  //Serial.begin(9600); // sets the serial port to 9600
  Serial.println("MQ3 warming up!");
  delay(2000); // allow the MQ3 to warm up
  pinMode(13, OUTPUT);
  pinMode(vib_pin,INPUT);
  
  // Set trigPin as OUTPUT and echoPin as INPUT
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop()

{
  sensorValue = analogRead(MQ3pin); // read analog input pin 0
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  sensorPer = (sensorValue/760)*100;
  Serial.print(sensorPer);
  Serial.println("%");
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

   // Set the trigPin HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, returning the time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  distance = duration * 0.034 / 2;

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Add a delay between readings
  delay(1000);
  


  int val;
  val=digitalRead(vib_pin);
  if(val==1)
  {
    Serial.print("Accident Detected\n");
    delay(1000);
  }
  else
  Serial.print("Not Detected\n");
  delay(1000);

  if(sensorValue < 450)
  {
    digitalWrite(13, HIGH);
    
  }
  else
  {
    digitalWrite(13, LOW);
    
  }
 

   while (bt.available())
   { 
      { data = bt.read();

        //Serial.println(data);
        }
      switch (data)
       {
          case 'F':                               
            //Serial.println("Forward");
            forward();
            break;
          case 'B':                
           //Serial.println("Reverse");

            reverse();
            break;
          case 'L':        
           //Serial.println("Left");
            left();
            break;         
          case 'R':                    
            //Serial.println("Right");
            right();
            break;       

          case 'S':                                           
            //Serial.println("Stop");
           stoprobot();
            break;
            case '1':
              analogWrite(EN1,100);
              analogWrite(EN2,100);
              break;
              case '2':
                analogWrite(EN1,120);
                analogWrite(EN2,120);
                break;
                case '3':
                analogWrite(EN1,140);
                analogWrite(EN2,140);
                break;
                case '4':
                analogWrite(EN1,160);
                analogWrite(EN2,160);
                break;
                case '5':
                analogWrite(EN1,180);
                analogWrite(EN2,180);
                break;
                case '6':
                analogWrite(EN1,200);
                analogWrite(EN2,200);
                break;
                case '7':
                analogWrite(EN1,220);
                analogWrite(EN2,220);
                break;
                case '8':
                analogWrite(EN1,240);
                analogWrite(EN2,240);
                break;
                case '9':
                analogWrite(EN1,255);
                analogWrite(EN2,255);
                break;              

          }
      }                                                             
       if (bt.available() < 0)                             
        {
         Serial.println("No Bluetooth Data ");         
        }

  delay(100); 

}

void forward() {


    digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(20);
}

void reverse() {


    digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(20);

}

void left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(20);

}

void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(20);

}

void stoprobot() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(20);

}
