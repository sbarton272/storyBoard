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
#define N_CLR_DETECT 4
#define MAX_DISTANCE 1000 // arbitrary

typedef char classification_t;

typedef struct {
    int r;
    int g;
    int b;
    classification_t classification;
} color_centroid_t;

typedef struct {
    int r;
    int g;
    int b;
} sensor_vals_t;

const int N_SENSOR_POLLS = 100;
const int greenPin = 4;
const int redPin = 3;
const int bluePin = 2;
const int colorSensePin = A0;
const color_centroid_t colorCentroids[N_CLR_DETECT] = {
    {750, 503, 805, 'b'},
    {728, 388, 737, 'w'},
    {769, 353, 763, 'r'},
    {771, 412, 703, 'g'}
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
#endif

    Serial.println( getCurrentColor() );

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

    setLEDs(HIGH, LOW, LOW); // red
    sensorVals.r = analogRead(colorSensePin);

    setLEDs(LOW, HIGH, LOW); // green
    sensorVals.g = analogRead(colorSensePin);

    setLEDs(LOW, LOW, HIGH); // blue
    sensorVals.b = analogRead(colorSensePin);

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
        default:
            // write back
            Serial.println(command);
            break;
    }


}
