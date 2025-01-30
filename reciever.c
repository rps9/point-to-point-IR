#include <IRremote.h> // >v3.0.0
#include <LiquidCrystal_I2C.h>
#include <PS2Keyboard.h>


#define PIN_RECV 2


bool clearNext;


bool printNext = true; // Declare the printNext flag globally
unsigned long lastReceivedTime = 0; // Variable to track the time of the last received character
LiquidCrystal_I2C lcd(0x27, 20,4);


String message = "";


void setup() {
 Serial.begin(9600); // Initialize the serial connection to print on the Serial Monitor of the Arduino IDE


 lcd.init();
 lcd.backlight();


 IRrecv IrReceiver(PIN_RECV); // Initialize the IR receiver with the input pin
 IrReceiver.enableIRIn(); // Enable the IR receiver


 clearNext = false;
}


void loop()
{
 lcd.setCursor(0,0);
 lcd.print("Message:");
 lcd.setCursor(0,1);
 if (IrReceiver.decode())
 {
    char receivedChar = (char)IrReceiver.decodedIRData.command;
   
    if (clearNext)
    {
      lcd.setCursor(0,0);
      lcd.print("Message:");
      lcd.setCursor(0,1);
      clearNext = false;
    }


    if (receivedChar != 999)
    {
      Serial.print(receivedChar);
      delay(20);
      message = message + receivedChar;
      lcd.print(message);


      IrReceiver.resume(); // Important, enables receiving the next IR signal
    }
    else
    {
      clearNext = true;
    }
 }
}
