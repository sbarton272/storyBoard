/* getting Ardunio serial to OLED working */

#include <SoftwareSerial.h>

/*====================================================
  Macros
  ====================================================*/

/* Serial */
#define SERIAL_BAUD (9600)
#define SOFT_SERIAL_BAUD (9600)
#define SOFT_RX_PIN (2)
#define SOFT_TX_PIN (3)
#define OLED_RESET_PIN (4)
#define OLED_RESET_DELAY (400)

/*====================================================
  Globals
  ====================================================*/

SoftwareSerial OLED = SoftwareSerial(SOFT_RX_PIN, SOFT_TX_PIN);

/*====================================================
  Set-up
  ====================================================*/

void setup() {
    char buf[2] = "A";
    byte zero = 0x00;

    /* Serial */
    pinMode(SOFT_RX_PIN, INPUT);
    pinMode(SOFT_TX_PIN, OUTPUT);
    OLED.begin(SOFT_SERIAL_BAUD);
    Serial.begin(SERIAL_BAUD);

    /* OLED Reset */
    pinMode(OLED_RESET_PIN, OUTPUT);
    delay(OLED_RESET_DELAY);
    digitalWrite(OLED_RESET_PIN, LOW);
    delay(OLED_RESET_DELAY);
    digitalWrite(OLED_RESET_PIN, HIGH);
    delay(OLED_RESET_DELAY);

    Serial.println("online");
    delay(6000);
    Serial.println("screen should be on");

    OLED.write(0xFF);  //MSB
    OLED.write(0xD7);  //LSB
    
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

    // media init
    OLED.write(zero);
    OLED.write(0x0C);
    OLED.write(zero);
    OLED.write(zero);
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

}

/*====================================================
  Loop
  ====================================================*/

void loop() {
    char buf[3] = "  ";
    byte zero = 0x00;

    Serial.println("Starting loop");

    OLED.write(0xFF);  //MSB
    OLED.write(0xD2);  //LSB

    //start point (x1, y1)
    OLED.write(zero);  //x1 (MSB)
    OLED.write(zero);  //x1 (LSB)
    OLED.write(zero);  //y1 (MSB)
    OLED.write(zero);  //y1 (LSB)

    //end point (x2, y2)
    OLED.write(zero);  //x2 (MSB)
    OLED.write(0x28);  //x2 (LSB)  
    OLED.write(zero);  //y2 (MSB)
    OLED.write(0x28);  //y2 (LSB)

    //hex colour value for yellow
    OLED.write(0xFF);  //colour (MSB)
    OLED.write(0xE0);  //colour (LSB)

    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

    // show all videos/images

    OLED.write(0xFF);
    OLED.write(0xB1);
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 2);
    Serial.print( buf );
    Serial.println(';');

    OLED.write(0xFF);
    OLED.write(0xB3);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(zero);
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

    delay(100);

    OLED.write(0xFF);
    OLED.write(0xB8);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(0x51);
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

    OLED.write(0xFF);
    OLED.write(0xBB);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(zero);
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

    delay(100);

    OLED.write(0xFF);
    OLED.write(0xB8);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(0x01);
    OLED.write(0x92);
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

    OLED.write(0xFF);
    OLED.write(0xBB);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(zero);
    OLED.write(zero);
    for(int i = 0; (i < 100) && !OLED.available(); i++ );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println(';');

    delay(2000);
}
