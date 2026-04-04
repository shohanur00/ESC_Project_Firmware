#include "foc_transforms.h"
#include "main.h"
#include <math.h>

#define INV_SQRT3 0.577350269f   // 1/sqrt(3)

// =========================
// Clarke Transform
// =========================
Clarke_t FOC_ClarkeTransform()
{
    Clarke_t out;

    out.Ialpha = current_a;
    out.Ibeta  = (current_a + 2.0f * current_b) * INV_SQRT3;

    return out;
}
