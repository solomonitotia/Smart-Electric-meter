//voltage 

//current

//gsm
#include<SoftwareSerial.h>
#include<LiquidCrystal.h>
SoftwareSerial SIM900 (8,7);
//LiquidCrystal
LiquidCrystal lcd (12, 11, 5, 4, 3, 2); //lcd display pin
//output pins
const int voltageSensor = A0; // voltage pin
const int currentSensor = A1; // current pin
//electricity bit
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
double sumWH = 0.00000;
float WH = 0;//energy consumption in watt hour 
double Voltage = 0;//AC supply peak voltage
double vrms = 0;//AC supply rms voltage
double current = 0;//load peak current
double irms = 0;//load rms current
double power = 0;//
//Billing
double sumKsh = 0.00000;//Total energy consumption in Ksh 
float Ksh = 0;//energy consumption in Ksh
void setup()
{
Serial.begin(9600);
SIM900.begin(9600);//gsm
lcd.begin(16,2);//LiquidCrystal
lcd.clear();
}
void loop(){
//getVPP();//get volateg being used
ReadNsendenergyCalculations();//read and send the total amount owed or billed in ksh
delay(3000);

}
//purpose is basically get the voltage being used

float getVPP(int pinValue)
{
  // pinValue = 0 means it is Voltage Input , pinValue = 1 means it is Current Input
  float result;
  int readValue;             // value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
   uint32_t start_time = millis();
   while((millis() - start_time) < 1000) //sample for 1 Sec
   {
        if(pinValue == 0)
       {
          // reading Voltage Input PIN
          readValue = analogRead(voltageSensor);
       }
       else if(pinValue == 1)
       {  
          // reading Current Input PIN
            readValue = analogRead(currentSensor);       
       }   
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0) / 1024.0;    
   return result;
}


void viewOnSerial()
{
  delay(1000);
  while (Serial.available())
  {
    SIM900.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (SIM900.available())
  {
    Serial.write(SIM900.read());//Forward what Software Serial received to Serial Port
  }
}

float getReading()

 { 

 float solution;
 if(sumWH <= 50)
  solution = (WH * 3.15);
 if(( sumWH > 50 ) && ( sumWH <= 100 ))
  solution = ( WH * 3.60 );
 if(( sumWH > 100 ) && (sumWH <= 250))
  solution = (WH * 4.25);
 if(sumWH > 250)
  solution = (WH * 5.20);
 return solution;
 
 }


void ReadNsendenergyCalculations()
{
    //VOLTAGE
  // getting voltage from Input PIN
 Voltage = getVPP(0);
 vrms = (Voltage / 2.0) * 0.707 * 575; //find total voltage
  Serial.print("VOLTAGE : "); 
  Serial.print(vrms);
  Serial.println("Volt"); 

 // getting current from Input PIN
 //CURRENT
 current = getVPP(1);
 irms = (current / 2.0) * 0.707 * 1000 / mVperAmp; 
  Serial.print("CURRENT :"); 
  Serial.print(irms);
  Serial.println("Amps");

 //getting power calculations
  //POWER CALCULATIONS
 power=(vrms * irms * 0.3099);
 WH = (power / 3600);
 sumWH = sumWH + WH;//total energy consumed
 Ksh = getReading();///ALTER THIS TO KSH
 sumKsh = sumKsh + Ksh ;
 
 //Serial.print("TOTAL ENERGY CONSUMED IN Ksh :"); 

 lcd.setCursor(1,0); // set the cursor at 1st col and 1st row
 lcd.print(vrms);
 lcd.print("v ");
 lcd.print(irms);
 lcd.print("A");
 lcd.setCursor(1,1); // set the cursor at 1st col and 2nd row
 lcd.print(power);                                                                                
 lcd.print("w");
 //delay(1000);
 lcd.clear(); // clear the screen
 lcd.setCursor(1,0); // set the cursor at 1st col and 1st row
 lcd.print(WH);
 lcd.print("WH ");                                                                                                 
 lcd.setCursor(1,1); // set the cursor at 1st col and 2nd row
 lcd.print(Ksh); //KENYA SHILLINGS 
 lcd.print("Rs.");
 //delay(1000);
 lcd.clear(); // clear the screen
 lcd.setCursor(1,0); // set the cursor at 1st col and 1st row 
 lcd.print(sumWH);
 lcd.print("Total WH ");
 lcd.setCursor(1,1); // set the cursor at 1st col and 2nd row
 lcd.print(sumKsh);
 lcd.print("Total Rs.");
 //delay(1000);
 lcd.clear();
 //sending the txt value
 Serial.println("AT+CMGF=1");
 SIM900.println("AT+CMGF=1");    // Setting the GSM Module in Text mode
  delay(1000); 
  Serial.print("AT"); 
  SIM900.println("AT");
  viewOnSerial();
  SIM900.println("AT+CMGS=\"+254719563542\"\r"); // Sending Energy Consumption to Customer's Mobile Number
  delay(1000); 
  SIM900.print("Dear Customer, Your Energy Consumption is KWH :");
  SIM900.print(sumWH);
  viewOnSerial();
  SIM900.print(" and Total Billing is KSH. ");//how is this Ksh gotten?
  SIM900.print(sumKsh);
  viewOnSerial();
  delay(100);
  SIM900.write(26); // ASCII code of CTRL+Z
  delay(100);

}
