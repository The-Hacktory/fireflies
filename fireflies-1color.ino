/*
 * Firefly Synchronization for the Arduino Pro Mini
 * 
 * Tim Bieniosek and Molly Baum / The Hacktory 2013
 * Adapted from code for Atmel AVR by Alex Weber
 * Algorithm from Tyrell, Auer, Bettstetter (2006)
 *
 */

#define ledRPort 10
#define ledGPort 11
#define ledBPort 5
#define sensorPort1 A0
#define sensorPort2 A1
#define sensorPort3 A2
#define sensorPort4 A3
//#define ledGndPort 4
#define FLASH_POWER 3000
int ambient_threshold = 3;
int interact_threshold = 12; 

#define SETUP_DELAY 3000

void setup() {
  
  Serial.begin(9600);
  delay(100); //get serial mon open
  //setup ports
  pinMode(ledRPort, OUTPUT);
  pinMode(ledGPort, OUTPUT);
  pinMode(ledBPort, OUTPUT);  
  //pinMode(ledGndPort, OUTPUT);  
  //digitalWrite(ledGndPort, LOW);
  pinMode(sensorPort1, INPUT); 
  pinMode(sensorPort2, INPUT);
  pinMode(sensorPort3, INPUT);
  pinMode(sensorPort4, INPUT);

  //test led
  flash();
  flash();
  flash();    
  interact();

  delay(SETUP_DELAY);

  //calibrate
  int cal[4] = {0,0,0,0};
  int val = 0;
  for(int i = 0; i < 64; i++)
  {
    val = get_light4();
    if(val>cal[0])
      cal[0] = val;
    else if(val>cal[1])
      cal[1] = val;
    else if(val>cal[2])
      cal[2] = val;
    else if(val>cal[3])
      cal[3] = val;
    delay(100);
  }
  ambient_threshold = cal[3] + ambient_threshold;
  interact_threshold = ambient_threshold + interact_threshold;
  if(interact_threshold > 1000)
    interact_threshold = 1000;

  Serial.println("Firefly 1.0 - OneColor");
  Serial.print("Ambient threshold = ");
  Serial.println(ambient_threshold);
  Serial.print("Interact threshold = ");
  Serial.println(interact_threshold);
  
  delay(SETUP_DELAY);
}

void lightup(int red, int time_ms)
{
  float rledval = red;
 // float gledval = green;
 // float bledval = blue;
  int rdiff = 0;
  int bdiff = 235;
  int gdiff = 40;
  
  if( rledval < 50 )
    rledval = rledval+(rledval/2);
    
 // if( gledval < gdiff )
 //   gdiff = gledval/48;
    
//  if( bledval < bdiff )
//    bdiff = floor(bledval/1.5);
    
  //rledval = rledval-rdiff >= 0 ? rledval-rdiff : 0;
 // bledval = bledval-bdiff >= 0 ? bledval-bdiff : 0;
 // gledval = gledval-gdiff >= 0 ? gledval-gdiff : 0;
  
  for(int i = 0; i < time_ms; i++)
  {
    analogWrite(ledRPort, rledval);
    delay(1);
    
    analogWrite(ledBPort, rledval);
    delay(1);
    
    analogWrite(ledGPort, rledval);
    delay(1);
  }
  
    analogWrite(ledRPort, 0);
    analogWrite(ledBPort, 0);
    analogWrite(ledGPort, 0);
}

void interact()
{  
  Serial.println("Interact");
  
  fade(255, 0, 0, 1); 
  delay(100);
    fade(255, 0, 0, 1); 
  delay(100);
    fade(255, 0, 0, 1); 
  delay(100);
    fade(255, 0, 0, 1); 
  delay(100);
    fade(255, 0, 0, 1); 
  delay(100);
}

void fade(int red, int green, int blue, int brightest_time)
{
  for( int i = 256; i > 0; i/=2 )
  {
    lightup(red-(i-1), i/6);
  }
  lightup(red, brightest_time);
  for( int i = 1; i < 256; i*=2 )
  {
    lightup(red-i, i/2);
  }
}

void flash()
{
  Serial.println("Flash!");
  int d = 10;
  fade(255, 255, 255, 400);
  delay(200);
}

int get_light4()
{
  int greatest = 0;
  int vals[4] = {0};
  vals[0] = analogRead( sensorPort1 );
  vals[1] = analogRead( sensorPort1 );
  vals[2] = analogRead( sensorPort1 );
  vals[3] = analogRead( sensorPort1 );
  Serial.print("Sensor vals ");
  Serial.println(vals[0]);
  Serial.println(vals[1]);
  Serial.println(vals[2]);
  Serial.println(vals[3]);
  
  for( int i = 0; i < 4; i++ )
  {
    if(vals[i] > greatest)
    {
      greatest = vals[i];
    }
  }
  return greatest;
}

int power = 0;
int blind = 0;
int light = 0;
void loop() {
  delay(200);
  
  //increment power level
  if( power > FLASH_POWER - 500 )
  {
    power += 50;
  }
  else if( power > FLASH_POWER - 1000 )
  {
    power += 50;
  }
  else if( power > FLASH_POWER - 2000 )
  {
    power += 50;
  }
  else if( power > FLASH_POWER - 3000 )
  {
    power += 100;
  }
  else
  {
    power += 100;
  }
  
  //read light value
  if( !blind )
  {
     light = get_light4();
     Serial.print("light=");
     Serial.println(light);
     
     if( light > interact_threshold )
     {
       interact();
     }
     if( light > ambient_threshold )
     {
       power += 1000;
       blind = 20;
     }
     //flash LED
    if( power > FLASH_POWER )
    {
      flash();
      power = 0;
    }
      
    Serial.print("Power=");
    Serial.println(power);
  }
  else if( blind > 0 )
  {
    Serial.print(".");
    blind--;
  }
  
}
