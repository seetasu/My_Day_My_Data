#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>

#define PIN            6
#define NUMPIXELS      20

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(10, 9);

const int chipSelect = 4;
int current_id = 0;
int current_question = 0;
int former_question = 0;
int current_answer = 0;
byte Ubuffer[18];

const int button1 = 2;
const int button2 = 3;
const int button3 = 4;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pixels.begin(); 
  pixels.show(); 
  
  pinMode(button1,INPUT);
  pinMode(button2,INPUT);
  pinMode(button3,INPUT);

}

void loop() {
//  for(int i = 0; i< 12; i ++){
//    pixels.setPixelColor(i,255,255,22); 
//    
//  }
  if(current_id == 0 && current_answer == 0 && current_question == 0){
    for(int i = 0; i <12; i++){
      pixels.setPixelColor(i,0,0,0);
    }
  }
    
  if(current_id == 0)//no user read
  {
    pixels.setPixelColor(4,255,231,6);
    pixels.show(); 
    if(current_question != 0)//while read a question
    {
    pixels.setPixelColor(4,255,231,6);
    pixels.show(); 
    delay(200);
    pixels.setPixelColor(4,0,0,0);
    pixels.show(); 
    delay(200);  
    current_question = 0;
    }
  }

   if(current_id != 0)
  {
    pixels.setPixelColor(4,0,0,0);
    pixels.setPixelColor(0,255,231,6);
    pixels.setPixelColor(1,255,231,6);
    pixels.setPixelColor(2,255,231,6);
    pixels.setPixelColor(3,255,231,6);
    pixels.setPixelColor(5,255,231,6);
    pixels.setPixelColor(6,255,231,6);
    pixels.setPixelColor(7,255,231,6);
    pixels.show(); 
  }

  if(current_id != 0 && current_question != 0)//when a id and a question is read, can get answer
  {
    get_answer();
  }

  if(current_answer != 0){
    pixels.setPixelColor(8,255,231,6);
    pixels.show();  
    current_question = 0;
    current_answer = 0;
  }


  if(current_question != 0){
    pixels.setPixelColor(8,0,0,0);
    pixels.show();  
  }
   
  //prepare key - all keys are set to FFFFFFFFFFFFFh at 
  //chip deliver from the factory.
  MFRC522::MIFARE_Key key;
  for(byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //Look for new cards
  if(! mfrc522.PICC_IsNewCardPresent()){
      return;
    }

  //select one of the ards
  if(! mfrc522.PICC_ReadCardSerial()){
      return;
    } 
 
  byte Usector = 0;
  byte UblockAddr = 1;
  byte status;
  byte UtrailerBlock = 3;
  byte Usize = sizeof(Ubuffer);
  
  
// Authenticate using key A
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, UtrailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) 
  {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(UblockAddr, Ubuffer, &Usize);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
//  Serial.print(F("Data in block ")); Serial.print(UblockAddr); Serial.println(F(":"));
//  dump_byte_array(Ubuffer, 16); Serial.println();
//  Serial.println();

  read_id();
  read_question();
  reset();
  
  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

}

void get_answer(){
  buttonState1 = digitalRead(button1);
  buttonState2 = digitalRead(button2);
  buttonState3 = digitalRead(button3);
  
  if(buttonState1 == HIGH && buttonState2 == LOW && buttonState3 == LOW){
    pixels.setPixelColor(11,255,0,37);
    pixels.show(); 
    Serial.println("current_answer: 1");
    current_answer = 1;
    delay(1000);
    pixels.setPixelColor(11,0,0,0);
    pixels.show();
  }

  if(buttonState1 == LOW && buttonState2 == HIGH && buttonState3 == LOW){
    pixels.setPixelColor(10,255,0,37); 
    pixels.show(); 
    Serial.println("current_answer: 2");
    current_answer = 2;
    delay(1000);
    pixels.setPixelColor(10,0,0,0);
    pixels.show(); 
  }

  if(buttonState1 == LOW && buttonState2 == LOW && buttonState3 == HIGH){
    pixels.setPixelColor(9,255,0,37);
    pixels.show();
    Serial.println("current_answer: 3");
    current_answer = 3;
    delay(1000);
    pixels.setPixelColor(9,0,0,0);
    pixels.show();
  }  
}

void read_id(){
  if(Ubuffer[0] == 0x01){
    current_id = 1;
//    Serial.println(F("User1"));
    Serial.print(F("current_id: "));
    Serial.println(current_id);
//    Serial.print(F("current_question: "));
//    Serial.println(current_question);
  }
  else if(Ubuffer[0] == 0x02){
    current_id = 2;  
//    Serial.println(F("User2"));
    Serial.print(F("current_id: "));
    Serial.println(current_id);
//    Serial.print(F("current_question: "));
//    Serial.println(current_question);
  }
}

void read_question(){
  if(Ubuffer[0] == 0xF1){
    current_question = 1;
//    Serial.println(F("Q1"));
//    Serial.print(F("current_id: "));
//    Serial.println(current_id);
    Serial.print(F("current_question: "));
    Serial.println(current_question);
  }  if(Ubuffer[0] == 0xF2){
    current_question = 2;
//    Serial.println(F("Q2 "));  
//    Serial.print(F("current_id: "));
//    Serial.println(current_id);
    Serial.print(F("current_question: "));
    Serial.println(current_question);
  }if(Ubuffer[0] == 0xF3){
    current_question = 3;
//    Serial.println(F("Q3"));
//    Serial.print(F("current_id: "));
//    Serial.println(current_id);
    Serial.print(F("current_question: "));
    Serial.println(current_question);
  }
}
void reset(){
  if(Ubuffer[0] == 0xE0){
    current_question = 0;
    current_answer = 0;
    current_id = 0;
//    Serial.println(F("Q1"));
//    Serial.print(F("current_id: "));
//    Serial.println(current_id);
//    Serial.print(F("current_question: "));
//    Serial.println(current_question);
  }   
}


void dump_byte_array(byte *buffer, byte bufferSize){
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

