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
    delay(5000);
    Serial.println("screen should be on");
}

/*====================================================
  Loop
  ====================================================*/

void loop() {
    char buf[2] = "A";

    OLED.write(0xFF);  //MSB
    OLED.write(0xD7);  //LSB
    while( !OLED.available() );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println();

    OLED.write(0xFF);  //MSB
    OLED.write(0xD2);  //LSB

    //start point (x1, y1)
    byte zero = 0x00;
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

    while( !OLED.available() );
    OLED.readBytes(buf, 1);
    Serial.print( buf );
    Serial.println();

    delay(1000);
}
