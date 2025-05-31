
#include <Arduino.h> 

class PIDAdaptive {
  public:
    float kp, ki, kd;
    float integral;
    float prev_error;
    float adapt_rate;

    PIDAdaptive(float _kp, float _ki, float _kd) {
      kp = _kp; ki = _ki; kd = _kd;
      integral = 0;
      prev_error = 0;
      adapt_rate = 0.01; // laju adaptasi parameter
    }

    float update(float error, float dt) {
      integral += error * dt;
      float derivative = (dt > 0) ? (error - prev_error) / dt : 0;

      // Penyesuaian adaptif ki dan kd
      ki += adapt_rate * abs(error) * ((error > 0) ? 1 : -1);
      kd += adapt_rate * abs(derivative) * ((derivative > 0) ? 1 : -1);

      // Batasi ki dan kd agar tetap dalam rentang wajar
      ki = constrain(ki, 0, 1);
      kd = constrain(kd, 0, 1);

      float output = kp * error + ki * integral + kd * derivative;
      prev_error = error;
      return output;
    }
};
