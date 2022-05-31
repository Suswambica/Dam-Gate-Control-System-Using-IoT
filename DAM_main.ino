
#include <Servo.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <SoftwareSerial.h>


#define echoPin 7
#define trigPin 4

bool status = LOW;
long old = 0;
String i2cData = "";
int pos = 0;    // variable to store the servo position
int levP[4] = {A0, A1, A2, A3};
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13 ;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial HSerial1(2, 3);
Servo gate1;
Servo gate2;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  for (int i = 0; i < 4; i++)pinMode(levP[i], INPUT_PULLUP);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  gate1.attach(5);
  gate2.attach(6);
  
  lcd.begin(16, 2);
  gate1.write(180);
  gate2.write(10);
  
  Wire.begin(8);
  Wire.onRequest(requestEvent);
  HSerial1.begin(9600);
  
}

void loop() {

  long value = 0;
  for (int i = 0; i < 4; i++) {
    int v = digitalRead(levP[i]);
    value = value + v;
  }
  value = 4 - value;
  int d = getDistance();

  i2cData = "level:" + String(value) + ",distance:" + String(d) + ";";
  lcd.setCursor(0, 0);
  lcd.print("                  ");
  String data = "Lev : " + String(value) + " Dis : " + String(d);
  lcd.setCursor(0, 0);
  lcd.print(data);
  //  Serial.print("DAM Level : ");
  //  Serial.println(value);
  if (value == 1 || value == 0 && status) {
    gate1.write(180);
    gate2.write(10);
    lcd.setCursor(0, 1);
    lcd.print("                               ");

  }
  else if (value == 2 && status) {
    Serial.println("DAM Gates are going to open");
    gate1.write(180);
    gate2.write(10);
    send_sms("7330864686", "DAM Gates are going to open");
//    delay(3000);
//    send_sms("7330864686", "DAM Gates are going to open");
    lcd.setCursor(0, 1);
    lcd.print("                               ");
    lcd.setCursor(0, 1);
    lcd.print("Gates : going to open");
    status = LOW;
  }
  else if (value == 3  && status) {
    //Serial.println("Opening Gate : 1");
    gateF1("open");
    lcd.setCursor(0, 1);
    lcd.print("                               ");
    lcd.setCursor(0, 1);
    lcd.print("Gate1 : Opened");
  }
  else if (value == 4  && status) {
    //Serial.println("Opening Gate : 2");
    gateF2("open");
    lcd.setCursor(0, 1);
    lcd.print("                               ");
    lcd.setCursor(0, 1);
    lcd.print("Gate2 : Opened");
  }

  if (value != old) {
    old = value;
    status = HIGH;
  }
  delay(500);
}

void gateF1(String s) {
  status = LOW;
  if (s.indexOf("close") != -1) {
    gate1.write(180);
    Serial.println("closing gate 1");
    return;
    for (int i = 10; i <= 180; i += 1) {
      gate1.write(i);
      delay(15);
    }
  }
  if (s.indexOf("open") != -1) {
    gate1.write(10);
    Serial.println("Opening gate 1");
    return ;
    for (int i = 180; i >= 10; i -= 1) {
      gate1.write(i);
      delay(15);
    }
  }
}

void gateF2(String s) {
  status = LOW;
  if (s.indexOf("open") != -1) {
    Serial.println("opening gate 2");
    gate2.write(180);
    return ;
    for (int i = 10; i <= 180; i += 1) {
      gate2.write(i);
      delay(15);
    }
  }
  if (s.indexOf("close") != -1) {
    Serial.println("closing gate 3");
    gate2.write(10);
    return ;
    for (int i = 180; i >= 10; i -= 1) {
      gate2.write(i);
      delay(15);
    }
  }
}
void upload(long x) {
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.print(String(x));  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */
}
void requestEvent() {
  Serial.println(i2cData);
  Wire.print(i2cData);  /*send string on request */
}

void send_sms(String no, String msg) {
  no = "91" + String(no);
  HSerial1.println("AT"); //Handshaking with SIM900
  updateSerial();
  delay(500);
  HSerial1.println("AT+CMGF=1"); // Configuring TEXT mode
  //Serial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  delay(500);
  HSerial1.println("AT+CMGS=\"+" + no + "\"");
  //Serial.println("AT+CMGS=\"+"+no+"\"");
  updateSerial();
  delay(500);
  HSerial1.print(msg);
  updateSerial();
  //Serial.print(msg);
  delay(500);
  HSerial1.write(26);
  updateSerial();
}

void updateSerial()
{
  return;

  delay(500);
  while (Serial.available())
  {
    HSerial1.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (HSerial1.available())
  {
    Serial.write(HSerial1.read());//Forward what Software Serial received to Serial Port
  }
}
int getDistance()
{
  long duration; // variable for the duration of sound wave travel
  int distance = 0;
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  if (distance > 90)distance = 90;
  return distance;
}
