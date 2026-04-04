#ifndef FOC_TRANSFORMS_H
#define FOC_TRANSFORMS_H

typedef struct {
    float Ialpha;
    float Ibeta;
} Clarke_t;

typedef struct {
    float Id;
    float Iq;
} Park_t;

// Clarke
Clarke_t FOC_ClarkeTransform();

// Park
Park_t ParkTransform(float Ialpha, float Ibeta, float theta);

// Inverse Park
void InvParkTransform(float Vd, float Vq, float theta, float *Valpha, float *Vbeta);

#endif
