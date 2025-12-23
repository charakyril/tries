#ifndef SENSOR_READING_H
#define SENSOR_READING_H

#include <string>
#include "types.h"

using namespace std;

struct SensorReading {
    string objectId;      // object identity string (may be "N/A" if unknown)
    string type;          // object category
    Position position;    // best-effort position
    float distance = -1.0f; // Manhattan distance
    float confidence = 0.0f; // 0.0 - 1.0
    string speed;         // speed string if available
    Direction direction;  // movement direction if available
    string signText;      // text for signs (if any)
    string lightColor;    // traffic light color (if any)
};

#endif // SENSOR_READING_H
