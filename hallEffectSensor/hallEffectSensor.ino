/* Testing the Hall Effect Sensor */

const int SENSOR_PIN = A0;
const int CALIBRATION_SAMPLES = 100;

int sensorThreshold;
int sensorVariance;

/*====================================================
  Set-up
  ====================================================*/

void setup() {
    Serial.begin(9600);
    pinMode(SENSOR_PIN, INPUT);

    // calibrate hall effect sensor
    // TODO figure out a better way to determine variance
    sensorThreshold = calibrateSensor(CALIBRATION_SAMPLES);
    sensorVariance = sensorThreshold / 10;

}

int calibrateSensor(int nSamples) {
    int avgVal, i;
    long int calibrationSum = 0;

    // calibrate sensor
    Serial.println("Calibrating");
    for( i = nSamples; i >= 0; --i) {
        calibrationSum += analogRead(SENSOR_PIN);
    }
    avgVal = calibrationSum / nSamples;
    Serial.print("Done calibrating: ");
    Serial.println(avgVal);

    return avgVal;
}

/*====================================================
  Loop
  ====================================================*/

void loop() {
    char overMagnet = '0';

    int sensorVal = analogRead(SENSOR_PIN);
    
    if( (sensorVal < (sensorThreshold - sensorVariance)) ) {
        overMagnet = '-';
    } else if ( (sensorVal > (sensorThreshold + sensorVariance)) ) {
        overMagnet = '+';
    }

    Serial.print( sensorVal );
    Serial.print( ' ' );
    Serial.println( overMagnet );


}
