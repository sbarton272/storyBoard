/* Spencer Barton, May 2014
 *
 * StoryBoard - An interactive story told with an OLED display and magnetic
 *  markers.
 *
 * This code sits on an Arduino or Teensy with these peripheries
 * - 3 Hall Effect Sensors
 * - OLED Display: https://www.4dsystems.com.au/product/uTOLED_20_G2/
 * 
 * Serial is connected to USB with SoftwareSerial to communicate with the OLED. 
 *
 * The three Hall Effect Sensors are laid out in a L to detect magnets in the 
 *  same config. Sensor values are polarity (+/-) or no magnet (0).
 */

 // TODO split this file up

#include <SoftwareSerial.h>

/*====================================================
  Macros
  ====================================================*/

/* Pin defs */
#define HALL_SENSOR_0 (A0)
#define HALL_SENSOR_1 (A1)
#define HALL_SENSOR_2 (A2)

/* Serial */
#define SERIAL_BAUD (9600)
#define SOFT_SERIAL_BAUD (9600)
#define SOFT_RX_PIN (2)
#define SOFT_TX_PIN (3)
#define OLED_RESET_PIN (4)
#define OLED_RESET_DELAY (400)

/* OLED Commands and Interface */
#define OLED_ACK (0x06)
#define N_SECTOR_BYTES (4)

/* Hall effect sensors */
#define N_CALIBRATION_SAMPLES (100)
#define VARIANCE_SCALER (4)
#define DEFAULT_SENSOR_THRESHOLD (512)
#define DEFAULT_SENSOR_VARIANCE (118)

#define N_ANIMATIONS (3)

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
    byte* cmds;
    int len;
    bool ack;
    bool ackVal;
} OLED_cmd_t;

typedef struct {
    byte setSectCmd[N_SECTOR_BYTES]; // TODO more memory efficient ways to pass around cmd
    magnet_tag_t tag;
    byte vidSector[N_SECTOR_BYTES];
    byte id;
} animation_t;

/*====================================================
  Globals
  ====================================================*/

SoftwareSerial OLED = SoftwareSerial(SOFT_RX_PIN, SOFT_TX_PIN);
int sensorThreshold;
int sensorVariance;

/*====================================================
  OLED
  ====================================================*/

// TODO i don't like this. They should all be const but need to 
//  figure out how to have variable array len in const

byte OLED_CLEAR_CMD[2] = {0xFF, 0xD7};
OLED_cmd_t OLED_CLEAR = {
    OLED_CLEAR_CMD,
    2,
    true,
    false
};

byte OLED_SS_TIMEOUT_CMD[4] = {0x00, 0x0C, 0x00, 0x00};
OLED_cmd_t OLED_SS_TIMEOUT = {
    OLED_SS_TIMEOUT_CMD,
    4,
    true,
    false
};

byte OLED_MEDIA_INIT_CMD[2] = {0xFF, 0xB1};
OLED_cmd_t OLED_MEDIA_INIT = {
    OLED_MEDIA_INIT_CMD,
    2,
    true,
    true
};

byte OLED_SET_SECT_CMD[6] = {0xFF, 0xB8, 0x00, 0x00, 0x00, 0x00};
OLED_cmd_t OLED_SET_SECT = {
    OLED_SET_SECT_CMD,
    6,
    true,
    false
};

byte OLED_RUN_VIDEO_CMD[6] = {0xFF, 0xBB, 0x00, 0x00, 0x00, 0x00};
OLED_cmd_t OLED_RUN_VIDEO = {
    OLED_RUN_VIDEO_CMD,
    6,
    true,
    false
};

byte OLED_SET_IMG_CMD[6] = {0xFF, 0xB3, 0x00, 0x00, 0x00, 0x00};
OLED_cmd_t OLED_SET_IMG = {
    OLED_SET_IMG_CMD,
    6,
    true,
    false
};

/*====================================================
  Animations
  ====================================================*/

const animation_t owlBlink1 = {
    {0xFF, 0xB8, 0x00, 0x00, 0x00, 0x51},
    {POLE_NEG, POLE_POS, POLE_POS},
    0,
    0
};

const animation_t owlDazed4 = {
    {0xFF, 0xB8, 0x00, 0x00, 0x00, 0x51},
    {POLE_POS, POLE_POS, POLE_POS},
    0,
    1
};

const animation_t owlCry5 = {
    {0xFF, 0xB8, 0x00, 0x00, 0x01, 0x92},
    {POLE_NEG, POLE_POS, POLE_NEG},
    0,
    2
};

const animation_t animations[N_ANIMATIONS] = { owlBlink1, owlDazed4, owlCry5 };

/*====================================================
  Set-up
  ====================================================*/

void setup() {
    /* Serial */
    pinMode(SOFT_RX_PIN, INPUT);
    pinMode(SOFT_TX_PIN, OUTPUT);
    OLED.begin(SOFT_SERIAL_BAUD);
    Serial.begin(SERIAL_BAUD);

    /* Sensors */
    pinMode(HALL_SENSOR_0, INPUT);
    pinMode(HALL_SENSOR_1, INPUT);
    pinMode(HALL_SENSOR_2, INPUT);

    /* OLED Reset */
    pinMode(OLED_RESET_PIN, OUTPUT);
    delay(OLED_RESET_DELAY);
    digitalWrite(OLED_RESET_PIN, LOW);
    delay(OLED_RESET_DELAY);
    digitalWrite(OLED_RESET_PIN, HIGH);

    Serial.println("Online");

    // calibrate hall effect sensor
    calibrateSensor(N_CALIBRATION_SAMPLES, HALL_SENSOR_0);

    // OLED media init, and stopscrolling
    // TODO may need to work on timing
    commandOLED(OLED_SS_TIMEOUT);
    Serial.println( commandOLED(OLED_MEDIA_INIT) );
    commandOLED(OLED_CLEAR);

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

    // returns byte, cast to char - assume same size
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
    char buf[2];
    int id;
    bool bIsNewId = true;

    magnet_tag_t magnetTag = readMagnetTag();
    id = idTag(magnetTag);
    /* if valid id then play animation */
    if ( (0 <= id) && (id < N_ANIMATIONS) ) {
        //playAnimation(id, bIsNewId);
        commandOLED(OLED_CLEAR);
        byte sect[N_SECTOR_BYTES] = {0x00, 0x00, 0x00, 0x51};
        commandOLED( makeSetSectorCmd(sect) );
        playAnimation(0, false);
    } else {
        commandOLED(OLED_SET_SECT);
        commandOLED(OLED_SET_IMG);
        //commandOLED(OLED_CLEAR);
    }

    /* Debug print */
    printMagnetPolarity(magnetTag.pole0);
    printMagnetPolarity(magnetTag.pole1);
    printMagnetPolarity(magnetTag.pole2);
    Serial.print(' ');
    Serial.print(id);
    Serial.println("; ");

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
  OLED
  ====================================================*/

/* play given animation with id. Requires valid id
 * if this id is new then need to set sector otherwise just run
 * returns boolean true for success
 */
bool playAnimation(int id, bool bNewId) {
    
    // require valid id
    if( !(0 <= id && id < N_ANIMATIONS) ) {
        return false;
    }

    // if is new id then set sector
    if ( bNewId ) {
        // set sector location for animation
        bool ack = commandOLED( makeSetSectorCmd(animations[id].setSectorCmd) );
        // if valid set-sector not set then have problem
        if ( !ack ) { 
            return false;
        }
    }

    return commandOLED( OLED_RUN_VIDEO );
}

/* makeSetSectorCmd Requires an animation with a sector and 
 *  makes a setSectorCmd using that sector. Uses OLED_SET_SECT
 *  as a template
 */

OLED_cmd_t makeSetSectorCmd( const byte* setSectCmd ) {
    
    OLED_cmd_t setSectorCmd;

    setSectorCmd.len = OLED_SET_SECT.len;
    setSectorCmd.ack = OLED_SET_SECT.ack;
    setSectorCmd.ackVal = OLED_SET_SECT.ackVal;
    setSectorCmd.cmds = setSectCmd;

    return setSectorCmd;

}

/* commandOLED sends given command to OLED and returns bool
 *  to signal success. 
 */
bool commandOLED(OLED_cmd_t cmd) {

    for(int i = 0; i < cmd.len; i++) { 
        OLED.write(cmd.cmds[i]);
    }

    if (cmd.ack) {
        return waitAckOLED(cmd.ackVal);
    }
    return true;
}

bool waitAckOLED(bool ackVal) {
    // TODO this function may block for too long
    char ack[2];
    while( !OLED.available() );
    if (ackVal) {
        OLED.readBytes(ack, 2);
    } else {
        OLED.readBytes(ack, 1);        
    }
    return (OLED_ACK == ack[0]) ;
}

/*====================================================
  Debug
  ====================================================*/

char waitOnUserInput(char * msg) {
    char buf;
    Serial.println(msg);
    Serial.println("Send any char");
    while( !Serial.available() );
    Serial.readBytes(&buf, 1);
    return buf;
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
