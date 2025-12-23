#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <vector>
#include <optional>
#include "types.h"
#include "sensor_reading.h"
#include "fusion.h"

using namespace std;

class NavigationSystem {
    private:
        vector<Position> targets;
        size_t current = 0;
        vector<SensorReading> lastFused;
        SensorFusionEngine fusionEngine;
    public:
        NavigationSystem() : fusionEngine(40) {}
        NavigationSystem(unsigned int minConf) : fusionEngine(minConf) {}

        void setTargets(const vector<Position>& t) { targets = t; current = 0; }
        optional<Position> getCurrentTarget() const {
            if (current < targets.size()) return targets[current];
            return {};
        }

        void receiveFusedReadings(const vector<SensorReading>& fused) { lastFused = fused; }

        // Fuse raw sensor data and store fused result
        vector<SensorReading> fuseSensorData(const vector<SensorReading>& raw) {
            auto fused = fusionEngine.fuseSensorData(raw);
            receiveFusedReadings(fused);
            return fused;
        }

        // Simple decision: return a direction towards current target and recommended action
        pair<Direction, string> makeDecision(const Position& myPos) {
            Direction moveDir{0,0};
            string action = "MAINTAIN";
            auto tgtOpt = getCurrentTarget();
            if (!tgtOpt.has_value()) return {moveDir, action};
            Position tgt = *tgtOpt;
            int dx = tgt.x - myPos.x;
            int dy = tgt.y - myPos.y;
            if (dx != 0) moveDir.x = (dx > 0 ? 1 : -1);
            else if (dy != 0) moveDir.y = (dy > 0 ? 1 : -1);

            // Check for obstacles or signals in fused readings
            bool decelerate = false;
            for (const auto& r : lastFused) {
                if (r.distance >= 0 && r.distance <= 2 && (r.type == "CAR" || r.type == "BIKE")) decelerate = true;
                if (!r.lightColor.empty() && (r.lightColor == "RED" || r.lightColor == "YELLOW") && r.distance >=0 && r.distance <= 3) decelerate = true;
                if (r.distance >=0 && abs(tgt.x - myPos.x) + abs(tgt.y - myPos.y) <= 5) decelerate = true; // approaching GPS
            }
            if (decelerate) action = "DECELERATE";
            else action = "ACCELERATE";

            return {moveDir, action};
        }

        // Move to next target if arrived
        void checkArrival(const Position& myPos) {
            if (current >= targets.size()) return;
            Position tgt = targets[current];
            if (abs(tgt.x - myPos.x) + abs(tgt.y - myPos.y) == 0) {
                ++current;
            }
        }
};

#endif // NAVIGATION_H
