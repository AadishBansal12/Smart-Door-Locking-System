#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(-1);



#define relay 4
//#define red 16
//#define green 15

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {9, 8, 7, 6};
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//LiquidCrystal_I2C lcd(0x27, 16, 2);


SoftwareSerial sim800l(2, 3);
int irsensor = A0;
int buzzer = 5;
int otp;
String otpstring = "";
int i = 0;
void setup()
{

  pinMode(irsensor, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  
  sim800l.begin(9600);
  Serial.begin(115200);
  
  sim800l.println("AT");
  updateSerial();
  pinMode(4, OUTPUT);
  
  digitalWrite(4, HIGH);
  delay(500);
  sim800l.println("AT+CSQ");
  updateSerial();
  delay(1000);

}
void updateSerial()
{
  delay(500);
  while (Serial.available()) {
    sim800l.write(Serial.read());
  }
  while (sim800l.available()) {
    Serial.write(sim800l.read());
  }
}
void loop()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //...............................
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 1);
  //display.setCursor(0, 0);
  
  display.println("   OTP Based");
  //display.setCursor(0, 0);
  display.println("   Door Lock");
  display.display();
  delay(2000);
  //.............................................
  if(digitalRead(irsensor) == 0)
  {
    digitalWrite(buzzer, HIGH);
    delay(50);
    }
  
  if(digitalRead(irsensor) == 1)
  {
    digitalWrite(buzzer, LOW);
  }
  delay(10);
  
  if (digitalRead(irsensor) == LOW)
  {
    otp = random(2000, 9999);
    otpstring = String(otp);
    Serial.println(otpstring);
    while (digitalRead(irsensor) == LOW) {}
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 1);
    
    display.println(" OTP Sent to");
    //display.setCursor(0, 1);
    display.println(" Your Mobile");
    display.display();
    delay(5000);
    
    Serial.print("OTP is ");
    delay(100);
    Serial.println(otpstring);
    delay(100);
    SendSMS();
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 1);
    display.println("Enter OTP :");
    display.display();
    delay(2000);
    getotp();
  }

}

void getotp() {

  String y = "";
  int a = y.length();
  while (a < 4)
  {
    char customKey = customKeypad.getKey();
    if (customKey) {
      display.setCursor(0, 1);
      y = y + customKey;
      display.print(y);
      a = y.length();
    }
  }
  Serial.print("Entered OTP is ");
  Serial.println(y);
  if (otpstring == y)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 1);
    display.println("Access Granted");
    //display.setCursor(0, 1);
    
    display.println("Door Opening");
    digitalWrite(relay, LOW);
    display.display();
    delay(2000);
    
    delay(5000);
    digitalWrite(relay, HIGH);
   
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 1);
    //display.setCursor(0, 0);
    display.println("Access Failed");
    //display.setCursor(0, 1);
    display.println("Try Again !!!");
    delay(3000);
    display.display();
    delay(2000);
  }


}
void SendSMS()
{
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print ("AT+CSMP=17,167,0,0\r");
  delay(500);
  sim800l.print("AT+CMGS=\"+919439459099\"\r");
  delay(500);
  sim800l.print("Your OTP is " + otpstring + " Just Type OTP And Unlock The Door");
  delay(500);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);

}