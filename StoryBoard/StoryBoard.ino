/* Spencer Barton, May 2014
 *
 * StoryBoard - An interactive story told with an OLED display and magnetic
 *  markers.
 *
 * This code sits on an Arduino or Teensy with these peripheries
 * - 3 Hall Effect Sensors
 * - OLED Display: https://www.4dsystems.com.au/product/uTOLED_20_G2/
 * 
 * The three Hall Effect Sensors are laid out in a L to detect magnets in the 
 *  same config. Sensor values are polarity (+/-) or no magnet (0).
 */

/*====================================================
  Structs
  ====================================================*/

typedef enum {
    POLE_POS, POLE_NEG, POLE_NONE
} magnet_polarity_t;

typedef struct {
    magnet_polarity_t pole0;
    magnet_polarity_t pole1;
    magnet_polarity_t pole2;
} magnet_tag_t;

typedef struct {
    magnet_tag_t tag;
    int vidSector;
    byte id;
} animation_t;

/*====================================================
  Macros
  ====================================================*/

const int HALL_SENSOR_0 = A0;
const int HALL_SENSOR_1 = A1;
const int HALL_SENSOR_2 = A2;

const int N_CALIBRATION_SAMPLES = 100;
const int VARIANCE_SCALER = 4;
const int DEFAULT_SENSOR_THRESHOLD = 512;
const int DEFAULT_SENSOR_VARIANCE = 118;

const int SERIAL_BAUD = 9600;
const int N_ANIMATIONS = 3;

/*====================================================
  Globals
  ====================================================*/

int sensorThreshold;
int sensorVariance;

/*====================================================
  Animations
  ====================================================*/

const animation_t owlBlink1 = {
    {POLE_NEG, POLE_POS, POLE_POS},
    0,
    0
};

const animation_t owlDazed4 = {
    {POLE_POS, POLE_POS, POLE_POS},
    0,
    1
};

const animation_t owlCry5 = {
    {POLE_NEG, POLE_POS, POLE_NEG},
    0,
    2
};

const animation_t animations[N_ANIMATIONS] = { owlBlink1, owlDazed4, owlCry5 };

/*====================================================
  Set-up
  ====================================================*/

void setup() {
    Serial.begin(9600);
    pinMode(HALL_SENSOR_0, INPUT);
    pinMode(HALL_SENSOR_1, INPUT);
    pinMode(HALL_SENSOR_2, INPUT);

    // calibrate hall effect sensor
    // TODO figure out a better way to determine variance
    calibrateSensor(N_CALIBRATION_SAMPLES, HALL_SENSOR_0);

}

/* calculate sensor values
 * sensorThreshold is the average when there is no magnet present
 * the variance is calculated with two reads. Measurements are taken
 * of the value when a magnet is placed nearby and again with the 
 * magnet flipped over. The scaled average distance from the 
 * threshold is then the variance
 */
int calibrateSensor(int nSamples, int sensorPin) {
    int polarityRead0, polarityRead1, dist0, dist1;
    char usrCmd;

    // set defaults, user can choose to override
    sensorThreshold = DEFAULT_SENSOR_THRESHOLD;
    sensorVariance  = DEFAULT_SENSOR_VARIANCE;

    Serial.println("Calibrating Sensor");

    usrCmd = waitOnUserInput("Do not place magnet nearby or send 'd' to use defaults");

    // get measurements if user does not want to use defaults
    if ( usrCmd != 'd' ) {
        sensorThreshold = calibrateSensorAverage(nSamples, sensorPin);
        
        waitOnUserInput("Place magnet nearby");
        polarityRead0 = calibrateSensorAverage(nSamples, sensorPin);

        waitOnUserInput("Flip magnet over and place nearby");
        polarityRead1 = calibrateSensorAverage(nSamples, sensorPin);

        // calculate variance
        dist0 = abs(sensorThreshold - polarityRead0);
        dist1 = abs(sensorThreshold - polarityRead1);
        sensorVariance = (dist0 + dist1) / (2 * VARIANCE_SCALER ); // halve for average and scale
    }
    
    // report results
    Serial.print("Done Calibrating SensorThreshold: ");
    Serial.print(sensorThreshold);
    Serial.print(" +/- ");
    Serial.println(sensorVariance);
    usrCmd = waitOnUserInput("Does this look good? ('y'/'n')");
    if (usrCmd == 'n') {
        // retry
        calibrateSensor(nSamples, sensorPin); 
    }

}

int calibrateSensorAverage(int nSamples, int sensorPin) {
    int avgVal, i;
    long int calibrationSum = 0;

    // calibrate sensor
    for( i = nSamples; i >= 0; --i) {
        calibrationSum += analogRead(sensorPin);
    }
    avgVal = calibrationSum / nSamples;

    return avgVal;
}

/*====================================================
  Loop
  ====================================================*/

void loop() {

    magnet_tag_t magnetTag = readMagnetTag();

    printMagnetPolarity(magnetTag.pole0);
    printMagnetPolarity(magnetTag.pole1);
    printMagnetPolarity(magnetTag.pole2);
    Serial.print(' ');
    Serial.print(idTag(magnetTag));
    Serial.println(';');

}

/*====================================================
  Tag Idenitifcation
  ====================================================*/

/* idTag - identify the animation with the given tag
 * returns -1 if no tag matched or if one of the poles
 * is POLE_NONE. If a match is made the index in 
 * the animations array is returned.
 */
int idTag(magnet_tag_t magnetTag) {
    int i;

    /* no match if any pole is POLE_NONE */
    if ( (magnetTag.pole0 == POLE_NONE) || 
         (magnetTag.pole1 == POLE_NONE) ||
         (magnetTag.pole2 == POLE_NONE) ) {
        return -1;
    }

    for(i = 0; i < N_ANIMATIONS; i++) {
        animation_t animation = animations[i];
        if( magnetTagEquals(magnetTag, animation.tag) ) {
            return i;
        }
    }

    return -1;
}

/* equlity function for magnet tags */
int magnetTagEquals(magnet_tag_t magnetTag0, magnet_tag_t magnetTag1) {
    if ( (magnetTag0.pole0 == magnetTag1.pole0) &&
         (magnetTag0.pole1 == magnetTag1.pole1) &&
         (magnetTag0.pole2 == magnetTag1.pole2) ) {
        return 1;
    }
    return 0;
}

magnet_tag_t readMagnetTag() {
    magnet_tag_t magnetTag;

    magnetTag.pole0 = readMagnet(HALL_SENSOR_0);
    magnetTag.pole1 = readMagnet(HALL_SENSOR_1);
    magnetTag.pole2 = readMagnet(HALL_SENSOR_2);

    return magnetTag;
}

magnet_polarity_t readMagnet(int sensorPin) {
    /* determine magnet polarity based off of sensorThreshold */
    
    magnet_polarity_t magnetPolarity = POLE_NONE;

    int sensorVal = analogRead(sensorPin);
    
    if( (sensorVal < (sensorThreshold - sensorVariance)) ) {
        magnetPolarity = POLE_NEG;
    } else if ( (sensorVal > (sensorThreshold + sensorVariance)) ) {
        magnetPolarity = POLE_POS;
    }

    return magnetPolarity;
}

/*====================================================
  Debug
  ====================================================*/

char waitOnUserInput(char * msg) {
    Serial.println(msg);
    Serial.println("Send any char");
    while( !Serial.available() );
    return Serial.read();
}

void printMagnetPolarity(magnet_polarity_t pole) {
    char poleSym;
    if (pole == POLE_NEG) {
        poleSym = '-';
    } else if (pole == POLE_POS) {
        poleSym = '+';
    } else {
        poleSym = '0';
    }

    Serial.print(poleSym);
}