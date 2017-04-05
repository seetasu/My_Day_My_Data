/*
 * Write personal data of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10    //Arduino Uno
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);    // Create MFRC522 instance.

void setup() {
        Serial.begin(9600);        // Initialize serial communications with the PC
        SPI.begin();                // Init SPI bus
        mfrc522.PCD_Init();        // Init MFRC522 card
        Serial.println(F("This code is for writing users on the tags "));
}

void loop() {
// Prepare key - all keys are set to FFFFFFFFFFFFh at
//        chip delivery from the factory.
        MFRC522::MIFARE_Key key;
        for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
        
        // Look for new cards
        if ( ! mfrc522.PICC_IsNewCardPresent()) {
                return;
        }

        // Select one of the cards
        if ( ! mfrc522.PICC_ReadCardSerial())    return;
        
         
        byte WriteBuffer[34] = {0x02}; // 
        byte ReadBuffer[18];
        byte size = sizeof(ReadBuffer);
        byte block;
        byte status, len;

                
        block = 1;
        //Serial.println(F("Authenticating using key A..."));
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
           Serial.print(F("PCD_Authenticate() failed: "));
           Serial.println(mfrc522.GetStatusCodeName(status));
           return;
        }
//        else Serial.println(F("PCD_Authenticate() success: "));
        
        // Write block
	status = mfrc522.MIFARE_Write(block, WriteBuffer, 16);
	if (status != MFRC522::STATUS_OK) {
	    Serial.print(F("MIFARE_Write() failed: "));
	    Serial.println(mfrc522.GetStatusCodeName(status));
            return;
	}
//      else Serial.println(F("MIFARE_Write() success: "));

      
// Read Info
  Serial.print(F("The current user is:  "));
  status = mfrc522.MIFARE_Read(block, ReadBuffer, &size);
  if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
  dump_byte_array(ReadBuffer, 1);

//  Serial.setTimeout(20000L);
//  Serial.println(F("Type your answer, ending with #"));
//  len = Serial.readBytesUntil("#",(char *) WriteBuffer, 30);
//  for(byte i =len; i < 30; i++) bufer[i] = ' ';


//if (Serial.available() > 0) {
//    // read the incoming byte:
//    //incomingByte = Serial.read();
//   // String str = Serial.readString();
////  Serial.print("faefawe");
//   int num = Serial.parseInt();
//
//    // look for the newline. That's the end of your
//    // sentence:
//    
//  // say what you got:
//      Serial.println("I received: ");
//      //Serial.println(incomingByte-48, DEC);
//      Serial.println(num, DEC);   
//      // Serial.println(str);
//  }

//  
//  if(Serial.available > 0 ){
//    int answer = Serial.read();
//    Serial.println("The answer is: ");
//    Serial.print(answer, DEC); 
//    }

        Serial.println(" "); 
        mfrc522.PICC_HaltA(); // Halt PICC
        mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
       
}
/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
