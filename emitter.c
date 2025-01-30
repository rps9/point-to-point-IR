#include <PS2Keyboard.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <Servo.h>


const int DataPin = 4; // Yellow
const int IRQpin =  2; // Green
#define PIN_SEND 5
int xAnalogRead = 0;
int yAnalogRead = 1;


char sentence[81];
int sentenceIndex = 0; // To keep track of the current index in the sentence array
int xServoPosition;
int yServoPosition;


PS2Keyboard keyboard;
IRsend irSender;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo xservo;
Servo yservo;




void sendSentence(const char* sentence) {
    int length = strlen(sentence);
    for (int i = 0; i < length; i++) {
        int asciiValue = int(sentence[i]);
        irSender.sendNEC(0x0101, asciiValue, false, 0); // Address of 101 for sending/starting
        delay(100); // Add a delay for better accuracy
    }
    irSender.sendNEC(0x0102, 0, false, 0); // Address of 102 for end
}


void setup() {
    keyboard.begin(DataPin, IRQpin, PS2Keymap_US);


    irSender.begin(PIN_SEND);


    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);


    xservo.attach(7);
    yservo.attach(6);
}


void loop() {
    if (keyboard.available()) {
        lcd.blink();
        char c = keyboard.read();


        if (c == PS2_ENTER) { // Enter key pressed
            lcd.noBlink();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Sending Message...");
            sendSentence(sentence);
            lcd.clear();
            lcd.print("Message Sent!");
            delay(1000);
            lcd.clear();
            sentenceIndex = 0; // Reset the sentence index
            memset(sentence, 0, sizeof(sentence)); // Clear the sentence array
        }
        else if (c == PS2_BACKSPACE) { // Backspace key pressed
            if (sentenceIndex > 0) {
                sentence[--sentenceIndex] = '\0'; // Remove the last character
                lcd.setCursor(sentenceIndex % 20, sentenceIndex / 20); // Set cursor to proper position
                lcd.print(" "); // Clear the character from display
                lcd.setCursor(sentenceIndex % 20, sentenceIndex / 20); // Move cursor back again
            }
        }
        else if(c == PS2_UPARROW || c == PS2_DOWNARROW || c == PS2_LEFTARROW || c == PS2_RIGHTARROW){
            lcd.noBlink();
            lcd.clear();
            lcd.setCursor(0, 1);
            lcd.print("Entering Servo Mode!");
            delay(1000);
            lcd.clear();
            lcd.print("Press Enter to Exit");
            while(c != PS2_ENTER)
            {
              c = keyboard.read();
              xServoPosition = analogRead(xAnalogRead);
              yServoPosition = analogRead(yAnalogRead);




              xServoPosition = map(xServoPosition, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
              yServoPosition = map(yServoPosition, 0, 1023, 0, 180);




              xservo.write(180-xServoPosition);
              yservo.write(yServoPosition);
            }
            lcd.clear();
        }
        else {
            if (sentenceIndex < sizeof(sentence) - 1) { // Ensure we don't exceed the array size
                sentence[sentenceIndex++] = c; // Add character to the sentence array
                lcd.print(c); // Print the character
                lcd.setCursor(sentenceIndex % 20, sentenceIndex / 20); // Move cursor to the next position
            } else {
                lcd.noBlink();
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Sending Message...");
                sendSentence(sentence);
                lcd.clear();
                lcd.print("Message Sent!");
                delay(1000);
                lcd.clear();
                sentenceIndex = 0; // Reset the sentence index
                memset(sentence, 0, sizeof(sentence)); // Clear the sentence array
            }
        }
    }
}
