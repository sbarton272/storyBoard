/* Color Sensor
 * RGB LED and Photoresistor
 *
 * D2 - Green
 * D3 - Red
 * D4 - Blue
 * A0 - ColorSensor
 * 
 */

// comment out to take out debug mode
#define SERIAL_DEBUG

const int N_SENSOR_POLLS = 100;
const int greenPin = 4;
const int redPin = 3;
const int bluePin = 2;
const int colorSensePin = A0;

void setup() {
    pinMode(greenPin, OUTPUT);
    pinMode(redPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    setLEDs(LOW, LOW, LOW);
    
    pinMode(colorSensePin, INPUT);  

#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    // handshake
    while (!Serial.available());
    char handshake = Serial.read();
    Serial.println(handshake);
#endif

}

void loop() {

#ifdef SERIAL_DEBUG
    pollSerialDebug();
#endif

}

/* poll serial for command 
 *  'r' - turn on red LED
 *  'b' - turn on gree LED
 *  'g' - turn on blue LED
 *  'o' - set all LEDs off
 *  's' - get light sensor value
 */
void pollSerialDebug() {
    char command = '\0';
    int colorVal = 0;
    int i;

    // hold until serial comes
    while (!Serial.available());
    command = Serial.read();

    // note low turns on LEDs
    switch (command) {
        case 'r':
            setLEDs(HIGH, LOW, LOW);
            Serial.println("red");
            break;
        case 'b':
            setLEDs(LOW, HIGH, LOW);
            Serial.println("blue");
            break;
        case 'g':
            setLEDs(LOW, LOW, HIGH);
            Serial.println("green");
            break;
        case 'o':
            setLEDs(LOW, LOW, LOW);
            Serial.println("off");
            break;
        case 's':
            colorVal = analogRead(colorSensePin);
            Serial.println(colorVal);
            break;
        case 't':
            for(i = 0; i < N_SENSOR_POLLS; i++) {
                colorVal = analogRead(colorSensePin);
                Serial.println(colorVal);
            }
            Serial.println("done");
            break;
        default:
            // write back
            Serial.println(command);
            break;
    }


}

void setLEDs(int red, int blue, int green) {
    // LEDs inverted
    digitalWrite(redPin, !red);
    digitalWrite(bluePin, !blue);
    digitalWrite(greenPin, !green);
}
