// Battery Charge / Discharge testing code - created 04/03/2023 by Brian Gardner
/* 
Code modified by Mark Niemann - 5/2/2023
Updated to include the usage of a LCD display.
LCD used in lieu of serial monitor
*/
#include <LiquidCrystal.h>

LiquidCrystal display(4,5,6,7,8,9); // Create the LCD object M.N.

int ps_relay = 2; //This is the pin that drives the relay for the charging power supply
int load_relay = 3; //This is the pin that drives the load for discharging the battery
int batteryPin = A0; //This identifies the pin used for reading battery voltage 
int state = 0; //This sets the state of the machine (three) 0 = charging the battery, 1 = wait and check voltage after charging, 2 = discharge testing, 3 = test complete
int batteryValue; //This is where we temporarily store the value from the A0 line that reads the battery voltage
float voltage;  //This is where we will temporarily store the battery voltage during each iteration of the test
float maths1;  //This is a floating point variable used for temprary math functions
float maths2;  //This is a floating point variable used for temprary math functions
float loadOhms = 2.6; //This is the resistance value of your load in ohms
float watts = 0; //This is where we will track how many watts have been dissapated by the load
int seconds = 0; //This is where we will track how long the test has been running

void setup() {
  // put your setup code here, to run once:


pinMode(ps_relay, OUTPUT); //Tells the IDE that the ps_relay pin is an output
pinMode(load_relay, OUTPUT); //Tells the IDE that the load_relay pin is an output
digitalWrite(ps_relay, HIGH); //if state = 0 then we're chraging the battery from the PS
digitalWrite(load_relay, HIGH); //if state = 0 then we're chraging the battery from the PS

// Serial.begin(9600);
display.begin(16,2); // Initalize the display M.N.
}

void loop() {
  // put your main code here, to run repeatedly:

  // Serial.print("Current State = ");
  // Serial.println(state);
  // Outputs the state to the top line of the LCD display M.N.
  display.setCursor(0,0);
  display.print("Current State = " + String(state));

  if (state == 0)  {

    // Serial.print("Charging - State = ");
    // Serial.println(state);  //Lets see what state we're in
    // Serial.println("Connecting the charger...");

    // Displays the current state on the top line and the progress on the bottom.
    display.setCursor(0,0);
    display.print("Charging - State = " + String(state));
    display.setCursor(0,1);
    display.print("Connecting the charger...");

    digitalWrite(ps_relay, LOW); //if state = 0 then we're chraging the battery from the PS
    batteryValue = analogRead(batteryPin);  //Read the battery voltage
    voltage = batteryValue * 0.0049;  //calculate battery voltage

    // Serial.print("Charging Voltage = ");

    voltage = voltage * 10; //We gotta do this because of our voltage divider network

    // Serial.println(voltage);  //Tell the world about that voltage
    display.setCursor(0,1);
    display.print("Charging Voltage = " + String(voltage));


    if (batteryValue >= 294) {  //If the battery voltage is greater than or equal to 14.4V then

      // Serial.println("Disconnecting the charger");
      display.setCursor(0,1);
      display.print("Disconneting the charger");
      digitalWrite(load_relay, HIGH);  //Just making sure the load is disconnected
      digitalWrite(ps_relay, HIGH); //Disconnect the battery and then
      state = 1;  //Change the state to 'resting' and exit the loop
    }
  }

  if (state == 1) {  //we're letting the battery rest for 30 seconds before testing it for early failure and / or moving on

    // Serial.print("Resting the battery - State = ");
    // Serial.println(state);  //Lets see what state we're in
    display.setCursor(0,0);
    display.print("Resting the battery - State = " + String(state));

    delay (30000);
    batteryValue = analogRead(batteryPin);  //Read the battery voltage

    // Serial.print("Resting Voltage = ");
    voltage = batteryValue * 0.0049;
    voltage = voltage * 10; //We gotta do this because of our voltage divider network
    // Serial.println(voltage);  //Tell the world about that voltage

    display.setCursor(0,1);
    display.print("Resting voltage = " + String(voltage));    

    if (voltage <= 12.5) {  //If the battery voltage is less than or equal to 12.5V then 

        // Serial.println("Early Failure");  //Tell the world it failed and then
        display.setCursor(0,1);
        display.print("Early Failure");

        while(true);  //This gets the arduino stuck in a infinie loop, essentially halting it

    } else {
      state = 2;  //The battery is full enough to run the discharge loop, set the state variable to 2, discharge testing

      // Serial.println(state);  //Lets see what state we're in
      display.setCursor(0,0);
      display.print("Battery Rested - State = " + String(state));
    }
  }

  if (state == 2) {
    digitalWrite(ps_relay, HIGH);  //Make sure the charger is disconnected
    digitalWrite(load_relay, LOW);  //Turn on the load

    // Serial.println("Load connected - Battery Discharging");
    display.setCursor(0,1);
    display.print("Load Connected - Battery Discharging");
    
    batteryValue = analogRead(batteryPin);  //Read the battery voltage
    voltage = batteryValue * 0.0049; //Convert the battery value to a voltage
    voltage = voltage * 10; //We gotta do this because of our voltage divider network
    
    // Serial.print("Voltage = ");
    // Serial.println(voltage);  //Tell the world about that voltage
    display.setCursor(0,1);
    display.print("Voltage = " + String(voltage));
    
    maths1 = voltage * voltage;  //Part of calculating watts is squaring the voltage
    maths2 = maths1 / loadOhms; //calulate how many watts we've dissapated if nothing changed for one hour
    maths1 = maths2 / 3600; //But, we're checking power dissipation every second, so divide that by 60
    watts = watts + maths1; //We add the power dissipated in the last one second to whatever we had before, accumulating tracked dissipated power
    delay (1000); //We're testing the the battery every second
    seconds++; //Increment seconds by 1

    if (voltage <= 12.5) {  //If the battery voltage is less than or equal to 12.5V then 
        
      state = 3;  //Test complete
      digitalWrite(ps_relay, HIGH);  //Make sure the charger is disconnected
      digitalWrite(load_relay, HIGH);  //Turn on the load
    
      // Serial.println(state);  //Lets see what state we're in    
      // Serial.println("TEST COMLPETE");
      display.setCursor(0,0);
      display.print("Testing Complete - State = " + String(state));
    
      watts = watts + watts;  // 12.5 volts is about %50 discharged so we'll double the watts before telling the user how many watts the battery capacity is
    
      // Serial.print("Watts = ");
      // Serial.println(watts);  //Display how many watts of capacity the battery has
      // Serial.print("Seconds = ");
      // Serial.println(seconds);  //Display how many seconds it took to get there

      display.setCursor(0,1);
      display.print("Watts = " + String(watts) + ", Seconds = " + String(seconds));
    
      while(true); //Stop the arduino
    } 
    //loop around and do it again till the voltage drops to 12.5
    }
}