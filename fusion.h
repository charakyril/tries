#ifndef FUSION_H
#define FUSION_H

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "sensor_reading.h"

using namespace std;

class SensorFusionEngine {
    public:
        unsigned int minConfidencePercent = 40; // default

        SensorFusionEngine(unsigned int minConf = 40) : minConfidencePercent(minConf) {}

        // Simple fusion: group by objectId; weighted average by confidence; drop low-confidence readings
        vector<SensorReading> fuseSensorData(const vector<SensorReading>& readings) const
        {
            map<string, vector<SensorReading>> grouped;
            for (const auto& r : readings) {
                string id = r.objectId;
                if (id.empty()) id = "N/A"; // group unknowns together
                grouped[id].push_back(r);
            }

            vector<SensorReading> out;
            for (const auto& kv : grouped) {
                const string& id = kv.first;
                const auto& group = kv.second;

                // compute confidence-weighted averages
                float wsum = 0.0f;
                SensorReading fused;
                fused.objectId = id;
                fused.type = "N/A";
                fused.position = Position{0,0};
                fused.distance = 0.0f;
                fused.confidence = 0.0f;
                fused.speed = "N/A";
                fused.signText = "N/A";
                fused.lightColor = "N/A";

                // For type and categorical fields we pick the highest-confidence observation
                float bestTypeConf = -1.0f;
                for (const auto& r : group) {
                    float c = r.confidence;
                    if (c > bestTypeConf && !r.type.empty()) {
                        fused.type = r.type;
                        bestTypeConf = c;
                    }
                    // accumulate weighted position/distance
                    fused.position.x += static_cast<int>(r.position.x * c);
                    fused.position.y += static_cast<int>(r.position.y * c);
                    fused.distance += r.distance * c;
                    fused.confidence += c;
                    wsum += c;
                    if (!r.speed.empty() && fused.speed == "N/A") fused.speed = r.speed;
                    if (!r.signText.empty() && fused.signText == "N/A") fused.signText = r.signText;
                    if (!r.lightColor.empty() && fused.lightColor == "N/A") fused.lightColor = r.lightColor;
                }
                if (wsum > 0.0f) {
                    fused.position.x = static_cast<int>(fused.position.x / wsum);
                    fused.position.y = static_cast<int>(fused.position.y / wsum);
                    fused.distance = fused.distance / wsum;
                    fused.confidence = fused.confidence / static_cast<float>(group.size());
                }

                // Convert to percent
                float confPercent = fused.confidence * 100.0f;

                // Keep reading if above threshold, or if type indicates bicycle (safety rule)
                if (confPercent >= static_cast<float>(minConfidencePercent) || fused.type == "BIKE") {
                    out.push_back(fused);
                }
            }
            return out;
        }
};

#endif // FUSION_H
