/* Testing the Hall Effect Sensor */

const int HALL_SENSOR_0 = A0;
const int HALL_SENSOR_1 = A1;
const int HALL_SENSOR_2 = A2;
const int N_CALIBRATION_SAMPLES = 100;
const int VARIANCE_SCALER = 4;

int sensorThreshold;
int sensorVariance;

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

    Serial.println("Calibrating Sensor");

    // get measurements
    waitOnUserInput("Do not place magnet nearby");
    sensorThreshold = calibrateSensorAverage(nSamples, sensorPin);
    
    waitOnUserInput("Place magnet nearby");
    polarityRead0 = calibrateSensorAverage(nSamples, sensorPin);

    waitOnUserInput("Flip magnet over and place nearby");
    polarityRead1 = calibrateSensorAverage(nSamples, sensorPin);

    // calculate variance
    dist0 = abs(sensorThreshold - polarityRead0);
    dist1 = abs(sensorThreshold - polarityRead1);
    sensorVariance = (dist0 + dist1) / (2 * VARIANCE_SCALER ); // halve for average and scale

    // report results
    Serial.print("Done Calibrating SensorThreshold: ");
    Serial.print(sensorThreshold);
    Serial.print(" +/- ");
    Serial.println(sensorVariance);
    waitOnUserInput("Does this look good?");

}

void waitOnUserInput(char * msg) {

    Serial.println(msg);
    Serial.println("Send any char");
    while( !Serial.available() );
    Serial.read();

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

    char magnetPolarity0 = readMagnet(HALL_SENSOR_0);
    char magnetPolarity1 = readMagnet(HALL_SENSOR_1);
    char magnetPolarity2 = readMagnet(HALL_SENSOR_2);

    Serial.println(';');

}

char readMagnet(int sensorPin) {
    /* determine magnet polarity based off of sensorThreshold */
    
    char magnetPolarity = '0';

    int sensorVal = analogRead(sensorPin);
    
    if( (sensorVal < (sensorThreshold - sensorVariance)) ) {
        magnetPolarity = '-';
    } else if ( (sensorVal > (sensorThreshold + sensorVariance)) ) {
        magnetPolarity = '+';
    }

    //Serial.print( sensorPin );
    //Serial.print( ' ' );
    //Serial.print( sensorVal );
    //Serial.print( ' ' );
    Serial.print( magnetPolarity );
    Serial.print( '\t' );

    return magnetPolarity;
}
