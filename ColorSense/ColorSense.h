#ifndef COLOR_SENSE_H
#define COLOR_SENSE_H

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

#endif

