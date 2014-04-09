                                                                                                                         /* Color Sensor
 * RGB LED and Photoresistor
 *
 * D2 - Green
 * D3 - Red
 * D4 - Blue
 * A0 - ColorSensor
 * 
 */

/* ===========================================
 * Constants
 * =========================================== */

// comment out to take out debug mode
#define SERIAL_DEBUG

#include "ColorSense.h"

// arduino compiler doesn't like #define sometimes
const int SENSOR_MEASURE_DELAY = 200;
const int N_SENSOR_POLLS = 100;


const int greenPin = 2;
const int redPin = 3;
const int bluePin = 4;
const int colorSensePin = A0;
// {r,g,b,classification}
// TODO may need to calibrate every time
// sensor readings noisy between color changes?
const color_centroid_t colorCentroids[N_CLR_DETECT] = {
    {292, 720, 711, 'r'},
    {386, 665, 729, 'g'},
    {503, 779, 694, 'b'},
    {330, 656, 662, 'w'}
};

/* ===========================================
 * Setup
 * =========================================== */


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

/* ===========================================
 * Loop
 * =========================================== */


void loop() {

#ifdef SERIAL_DEBUG
    pollSerialDebug();
#else
    Serial.print("Color: ");
    Serial.println( getCurrentColor() );
#endif

}

/* ===========================================
 * Classify Color
 * =========================================== */

classification_t getCurrentColor() {
    return classifyColor(getSensorVals());
}

classification_t classifyColor(sensor_vals_t sensorVals) {
    /* look for closest centroid for classification */
    int i;
    int distance;
    int minDistance = MAX_DISTANCE;
    classification_t classification = '\0';

    // iter though centroid choices and find closet to choose as classification
    for( i = 0; i < N_CLR_DETECT; i++) {

        distance = getDistance(sensorVals, colorCentroids[i]);

        if( distance < minDistance ) {
            minDistance = distance;
            classification = colorCentroids[i].classification;
        }

    }

    return classification;

}

int getDistance(sensor_vals_t sensorVals, color_centroid_t colorCentroid) {
    // get abs distance from centroid distance
    return abs(sensorVals.r - colorCentroid.r) + abs(sensorVals.g - colorCentroid.g) + abs(sensorVals.b - colorCentroid.b);
};


/* ===========================================
 * Helpers
 * =========================================== */

void setLEDs(int red, int green, int blue) {
    // LEDs inverted
    digitalWrite(redPin, !red);
    digitalWrite(greenPin, !green);
    digitalWrite(bluePin, !blue);
}

sensor_vals_t getSensorVals() {
    // set given led colors and return sensor values
    sensor_vals_t sensorVals;

    delay(SENSOR_MEASURE_DELAY);
    setLEDs(HIGH, LOW, LOW); // red
    delay(SENSOR_MEASURE_DELAY);
    sensorVals.r = analogRead(colorSensePin);
    Serial.println(sensorVals.r);
    delay(SENSOR_MEASURE_DELAY);

    setLEDs(LOW, HIGH, LOW); // green
    delay(SENSOR_MEASURE_DELAY);
    sensorVals.g = analogRead(colorSensePin);
    Serial.println(sensorVals.g);
    delay(SENSOR_MEASURE_DELAY);

    setLEDs(LOW, LOW, HIGH); // blue
    delay(SENSOR_MEASURE_DELAY);
    sensorVals.b = analogRead(colorSensePin);
    Serial.println(sensorVals.b);
    delay(SENSOR_MEASURE_DELAY);
    
    setLEDs(LOW, LOW, LOW); // off

    return sensorVals;
}

/* ===========================================
 * Debug
 * =========================================== */

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
        case 'g':
            setLEDs(LOW, HIGH, LOW);
            Serial.println("green");
            break;
        case 'b':
            setLEDs(LOW, LOW, HIGH);
            Serial.println("blue");
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
        case 'c':
            Serial.println("classify (r,g,b)c ");
            Serial.println( getCurrentColor() );
            break;
        default:
            // write back
            Serial.println(command);
            break;
    }


}
