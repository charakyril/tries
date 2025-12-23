#include <iostream>
#include <string>
#include <cctype>
#include <cmath>
#include "types.h"
#include "world.h"
#include "sensor.h"
#include <chrono>

using namespace std;

//Global variable ticks//
unsigned int ticks = 0;

//Global variable gps_pos//
Position gps_pos;

/*void printHelp ()
{
    cout << "Self −Driving Car Simulation" << endl;
    cout << "Usage: " << endl;
    cout << " −−seed <n>
    Random seed ( default : current time)" << endl;
    cout << " −−dimX <n>
    cout << " −−dimY <n>
    cout << " −−numMovingCars <n>
    cout << " −−numMovingBikes <n>
    cout << " −−numParkedCars <n>
    cout << " −−numStopSigns <n>
    cout << " −−numTrafficLights <n>
    cout << " −−simulationTicks <n>
    World width ( default : 40)" << endl;
    World height ( default : 40)" << endl;
    Number of moving cars ( default : 3)" << endl;
    Number of moving bikes ( default : 4)" << endl;
    Number of parked cars ( default : 5)" << endl;
    Number of stop signs (default : 2)" << endl;
    Number of traffic lights ( default : 2)" << endl;
    Maximum simulation ticks ( default : 100)" << endl;
    cout << " −−minConfidenceThreshold <n> Minimum confidence cutoff ( default : 40)" << endl;
    cout << " −−gps <x1> <y1> [x2 y2 . . .] GPS target coordinates (required)" << endl;
    cout << " −−help
    Show this help message" << endl << endl;
    cout << "Example usage: " << endl;
    cout << " ./ oopproj_2025 −−seed 12 −−dimY 50 −−gps 10 20 32 15" << endl;
}*/

// Shared types (Position, Direction) are defined in types.h

#include "navigation.h"
#include "fusion.h"
#include "sensor_reading.h"
#include "world_manager.h"
#include <random>

//----CLASS FOR SelfDrivingCar----//
class SelfDrivingCar
{
    protected:
        Direction car_direction;
        //SPEED = STOPPED, HALF_SPEED, FULL_SPEED
        string speed;
        // position
        Position position{0,0};
        //ALL SENSORS
        Lidar ld_sensor;
        Radar rd_sensor;
        Camera cam_sensor;
        //class navigation system
        NavigationSystem nav_system;
    public:
        //Constructor
        SelfDrivingCar(Direction dir, const string& sp, const Position& pos, Lidar lidar, Radar radar, Camera camera, NavigationSystem nav)
        : car_direction(dir), speed(sp), position(pos), ld_sensor(lidar), rd_sensor(radar), cam_sensor(camera), nav_system(nav)
        {
            cout << "I made the self driving car\n";
            // initialize camera's notion of the car position
            cam_sensor.position = position;
        }
        //Destructor
        ~SelfDrivingCar() 
        {
            cout << "No self driving car\n";
        }

        const Position& getPosition() const { return position; }

        // Collect raw sensor readings from the world
        vector<SensorReading> collectSensorData(const World& world, mt19937_64& rng) {
            vector<SensorReading> out;
            uniform_real_distribution<float> noise(-0.05f, 0.05f);

            for (auto p : world.objects) {
                Position objPos = p->getPosition();
                int dist = abs(objPos.x - position.x) + abs(objPos.y - position.y);

                // LIDAR (360-degree)
                if (dist <= static_cast<int>(ld_sensor.RANGE)) {
                    SensorReading r;
                    r.objectId = p->getID(); r.type = p->getType(); r.position = objPos; r.distance = static_cast<float>(dist);
                    float base = static_cast<float>(ld_sensor.ACCURACY) / 100.0f;
                    r.confidence = max(0.0f, min(1.0f, base * (1.0f - (float)dist / (ld_sensor.RANGE + 1)) + noise(rng)));
                    out.push_back(r);
                }

                // RADAR (straight ahead, only moving objects)
                if (dist <= static_cast<int>(rd_sensor.RANGE)) {
                    // check moving
                    if (dynamic_cast<MovingObjects*>(p) != nullptr) {
                        SensorReading r;
                        r.objectId = p->getID(); r.type = p->getType(); r.position = objPos; r.distance = static_cast<float>(dist);
                        float base = static_cast<float>(rd_sensor.ACCURACY) / 100.0f;
                        r.confidence = max(0.0f, min(1.0f, base * (1.0f - (float)dist / (rd_sensor.RANGE + 1)) + noise(rng)));
                        out.push_back(r);
                    }
                }

                // CAMERA (front 7x7, simplified as within range)
                if (dist <= static_cast<int>(cam_sensor.RANGE)) {
                    SensorReading r;
                    r.objectId = p->getID(); r.type = p->getType(); r.position = objPos; r.distance = static_cast<float>(dist);
                    float base = static_cast<float>(cam_sensor.ACCURACY) / 100.0f;
                    r.confidence = max(0.0f, min(1.0f, base * (1.0f - (float)dist / (cam_sensor.RANGE + 1)) + noise(rng)));
                    // if traffic light, try to get its color
                    if (auto tl = dynamic_cast<TRAFFIC_LIGHTS*>(p)) r.lightColor = tl->getColour();
                    if (dynamic_cast<TRAFFIC_SIGNS*>(p)) r.signText = "STOP"; // basic
                    out.push_back(r);
                }
            }
            return out;
        }

        // sync with navigation system by fusing data
        vector<SensorReading> syncNavigationSystem(NavigationSystem& nav, const vector<SensorReading>& raw) {
            auto fused = nav.fuseSensorData(raw);
            return fused;
        }

        // execute movement: apply decision and update position
        // returns true if moved outside world (end condition)
        bool executeMovement(const Direction& dir, const string& action, const World& world) {
            // adjust speed
            if (action == "ACCELERATE") {
                if (speed == "STOPPED") speed = "HALF_SPEED";
                else if (speed == "HALF_SPEED") speed = "FULL_SPEED";
            } else if (action == "DECELERATE") {
                if (speed == "FULL_SPEED") speed = "HALF_SPEED";
                else if (speed == "HALF_SPEED") speed = "STOPPED";
            }

            int steps = 0;
            if (speed == "FULL_SPEED") steps = 2;
            else if (speed == "HALF_SPEED") steps = 1;

            for (int i = 0; i < steps; ++i) {
                int nx = position.x + static_cast<int>(dir.x);
                int ny = position.y + static_cast<int>(dir.y);
                if (nx < 0 || ny < 0 || static_cast<unsigned int>(nx) >= world.getDimX() || static_cast<unsigned int>(ny) >= world.getDimY()) {
                    // attempted to go out of bounds
                    return true;
                }
                position.x = nx; position.y = ny;
            }
            // update camera position
            cam_sensor.position = position;
            // inform nav system about arrival check
            nav_system.checkArrival(position);
            return false;
        }

        void describe() const {
            cout << "SelfDrivingCar at (" << position.x << "," << position.y << ") speed=" << speed << "\n";
        }

        NavigationSystem& getNavigation() { return nav_system; }
};


//GPS targets as arguments from user
int main(int argc, char* argv[]) {

   /* if(argc < 3 || argc > 3)
    {
        cout << "Right usage: <executable progam>, <X1>, <Y1>,..." << endl;
    }
    if(argc == 3)
    {
        gps_pos.x = stof(argv[1]);
        gps_pos.y = stof(argv[2]);
        cout << "I took coordinates: " << gps_pos.x << " " << gps_pos.y << endl;
    }*/

    // Defaults
    unsigned long long seed = static_cast<unsigned long long>(
    chrono::system_clock::now().time_since_epoch().count());
    unsigned int dimX = 40;
    unsigned int dimY = 40;
    unsigned int numMovingCars = 3;
    unsigned int numMovingBikes = 4;
    unsigned int numParkedCars = 5;
    unsigned int numStopSigns = 2;
    unsigned int numTrafficLights = 2;
    unsigned int simulationTicks = 100;
    unsigned int minConfidenceThreshold = 40; // percent
    vector<Position> gpsTargets;

    auto printHelp = []() {
        cout << "Self-Driving Car Simulation" << endl;
        cout << "Usage:" << endl;
        cout << "  --seed <n>              " << "Random seed (default: current time)" << endl;
        cout << "  --dimX <n>              " << "World width (default: 40)" << endl;
        cout << "  --dimY <n>              " << "World height (default: 40)" << endl;
        cout << "  --numMovingCars <n>     " << "Number of moving cars (default: 3)" << endl;
        cout << "  --numMovingBikes <n>    " << "Number of moving bikes (default: 4)" << endl;
        cout << "  --numParkedCars <n>     " << "Number of parked cars (default: 5)" << endl;
        cout << "  --numStopSigns <n>      " << "Number of stop signs (default: 2)" << endl;
        cout << "  --numTrafficLights <n>  " << "Number of traffic lights (default: 2)" << endl;
        cout << "  --simulationTicks <n>   " << "Maximum simulation ticks (default: 100)" << endl;
        cout << "  --minConfidenceThreshold <n> " << "Min confidence cutoff (default: 40)" << endl;
        cout << "  --gps <x1> <y1> [x2 y2 ...] " << "GPS target coordinates (required)" << endl;
        cout << "  --help                  " << "Show this help message" << endl << endl;
        cout << "Example usage:" << endl;
        cout << "  ./oopproj_2025 --seed 12 --dimY 50 --gps 10 20 32 15" << endl;
    };

    // Parse args
    for (int i = 1; i < argc; ++i) {
        string a = argv[i];

        if (a == "--help") {
            printHelp();
            return 0;
        } else if (a == "--seed") {
            if (i + 1 >= argc) { cerr << "--seed requires a value\n"; return 1; }
            try { seed = stoull(argv[++i]); } catch (...) { cerr << "Invalid --seed\n"; return 1; }
        } else if (a == "--dimX") {
            if (i + 1 >= argc) { cerr << "--dimX requires a value\n"; return 1; }
            try { dimX = stoul(argv[++i]); } catch (...) { cerr << "Invalid --dimX\n"; return 1; }
        } else if (a == "--dimY") {
            if (i + 1 >= argc) { cerr << "--dimY requires a value\n"; return 1; }
            try { dimY = stoul(argv[++i]); } catch (...) { cerr << "Invalid --dimY\n"; return 1; }
        } else if (a == "--numMovingCars") {
            if (i + 1 >= argc) { cerr << "--numMovingCars requires a value\n"; return 1; }
            try { numMovingCars = stoul(argv[++i]); } catch (...) { cerr << "Invalid --numMovingCars\n"; return 1; }
        } else if (a == "--numMovingBikes") {
            if (i + 1 >= argc) { cerr << "--numMovingBikes requires a value\n"; return 1; }
            try { numMovingBikes = stoul(argv[++i]); } catch (...) { cerr << "Invalid --numMovingBikes\n"; return 1; }
        } else if (a == "--numParkedCars") {
            if (i + 1 >= argc) { cerr << "--numParkedCars requires a value\n"; return 1; }
            try { numParkedCars = stoul(argv[++i]); } catch (...) { cerr << "Invalid --numParkedCars\n"; return 1; }
        } else if (a == "--numStopSigns") {
            if (i + 1 >= argc) { cerr << "--numStopSigns requires a value\n"; return 1; }
            try { numStopSigns = stoul(argv[++i]); } catch (...) { cerr << "Invalid --numStopSigns\n"; return 1; }
        } else if (a == "--numTrafficLights") {
            if (i + 1 >= argc) { cerr << "--numTrafficLights requires a value\n"; return 1; }
            try { numTrafficLights = stoul(argv[++i]); } catch (...) { cerr << "Invalid --numTrafficLights\n"; return 1; }
        } else if (a == "--simulationTicks") {
            if (i + 1 >= argc) { cerr << "--simulationTicks requires a value\n"; return 1; }
            try { simulationTicks = stoul(argv[++i]); } catch (...) { cerr << "Invalid --simulationTicks\n"; return 1; }
        } else if (a == "--minConfidenceThreshold") {
            if (i + 1 >= argc) { cerr << "--minConfidenceThreshold requires a value\n"; return 1; }
            try { minConfidenceThreshold = stoul(argv[++i]); } catch (...) { cerr << "Invalid --minConfidenceThreshold\n"; return 1; }
        } else if (a == "--gps") {
            // collect trailing tokens until next --flag or end
            int j = i + 1;
            vector<string> tokens;
            while (j < argc && string(argv[j]).rfind("--", 0) != 0) {
                tokens.push_back(argv[j++]);
            }
            if (tokens.empty()) { cerr << "--gps requires at least one pair of coordinates\n"; return 1; }
            if (tokens.size() % 2 != 0) { cerr << "--gps requires x y pairs (even number of values)\n"; return 1; }
            for (size_t k = 0; k < tokens.size(); k += 2) {
                try {
                    int x = stoi(tokens[k]);
                    int y = stoi(tokens[k + 1]);
                    gpsTargets.push_back(Position{x, y});
                } catch (...) { cerr << "Invalid GPS coordinates\n"; return 1; }
            }
            i = j - 1; // advance main loop
        } else {
            cerr << "Unknown argument: " << a << "\n";
            printHelp();
            return 1;
        }
    }

    if (gpsTargets.empty()) {
        cerr << "Error: at least one GPS target must be provided with --gps\n";
        printHelp();
        return 1;
    }

    // Seed RNG (example)
    mt19937_64 rng(seed);

    // Echo parsed configuration
    cout << "Configuration:" << endl;
    cout << "  seed: " << seed << endl;
    cout << "  dimX x dimY: " << dimX << " x " << dimY << endl;
    cout << "  movingCars: " << numMovingCars << ", movingBikes: " << numMovingBikes << endl;
    cout << "  parkedCars: " << numParkedCars << ", stopSigns: " << numStopSigns << ", trafficLights: " << numTrafficLights << endl;
    cout << "  simulationTicks: " << simulationTicks << endl;
    cout << "  minConfidenceThreshold: " << minConfidenceThreshold << endl;
    cout << "  GPS targets (" << gpsTargets.size() << "):";
    for (const auto& p : gpsTargets) cout << " (" << p.x << "," << p.y << ")";
    cout << endl;

    // Build world and place objects
    World world(dimX, dimY);

    // pick a random position for the self-driving car that is free
    uniform_int_distribution<int> xdist(0, dimX-1);
    uniform_int_distribution<int> ydist(0, dimY-1);
    Position carPos;
    do {
        carPos.x = xdist(rng); carPos.y = ydist(rng);
    } while (!world.isCellFree(carPos.x, carPos.y));

    // Create sensors and navigation
    Lidar lidar("LIDAR", 9, 9, 87, 0.0f, "N/A", 0.99f);
    Radar radar("RADAR", 12, 12, 95, 0.0f, "STOPPED", Direction{0,1}, 0.99f);
    Camera camera("CAM", 7, 7, 95, "N/A", carPos, "N/A", 0.95f, "STOPPED", Direction{0,1}, "N/A", "GREEN");

    NavigationSystem nav(minConfidenceThreshold);
    nav.setTargets(gpsTargets);

    SelfDrivingCar car(Direction{0,1}, "STOPPED", carPos, lidar, radar, camera, nav);

    // populate world with parked cars, signs, lights, moving vehicles
    for (unsigned int i = 0; i < numParkedCars; ++i) {
        Position p;
        do { p.x = xdist(rng); p.y = ydist(rng); } while (!world.isCellFree(p.x, p.y));
        world.addObject(new STAT_VEH('P', "PARKED", p.x, p.y, "PARKED", 0));
    }
    for (unsigned int i = 0; i < numStopSigns; ++i) {
        Position p;
        do { p.x = xdist(rng); p.y = ydist(rng); } while (!world.isCellFree(p.x, p.y));
        world.addObject(new TRAFFIC_SIGNS('S', "SIGN", p.x, p.y, "SIGN", "STOP", 0));
    }
    for (unsigned int i = 0; i < numTrafficLights; ++i) {
        Position p;
        do { p.x = xdist(rng); p.y = ydist(rng); } while (!world.isCellFree(p.x, p.y));
        world.addObject(new TRAFFIC_LIGHTS('L', "LIGHT", p.x, p.y, "LIGHT", "RED", 0));
    }
    // moving cars
    vector<Direction> dirs = {{1,0},{-1,0},{0,1},{0,-1}};
    uniform_int_distribution<int> dirDist(0, static_cast<int>(dirs.size())-1);
    for (unsigned int i = 0; i < numMovingCars; ++i) {
        Position p;
        do { p.x = xdist(rng); p.y = ydist(rng); } while (!world.isCellFree(p.x, p.y));
        Direction d = dirs[dirDist(rng)];
        world.addObject(new CARS('C', "CAR", p.x, p.y, "HALF_SPEED", d, "CAR", 0));
    }
    for (unsigned int i = 0; i < numMovingBikes; ++i) {
        Position p;
        do { p.x = xdist(rng); p.y = ydist(rng); } while (!world.isCellFree(p.x, p.y));
        Direction d = dirs[dirDist(rng)];
        world.addObject(new BIKES('B', "BIKE", p.x, p.y, "HALF_SPEED", d, "BIKE", 0));
    }

    // Simulation loop
    bool outOfBounds = false;
    for (unsigned int t = 0; t < simulationTicks && !outOfBounds; ++t) {
        cout << "\nTick: " << t << "\n";
        world.printAround(car.getPosition(), 4);
        car.describe();

        // update world objects (moving vehicles, lights etc.)
        world.updateTick();

        // collect sensor data
        auto raw = car.collectSensorData(world, rng);
        // fuse and feed to navigation
        auto fused = car.syncNavigationSystem(nav, raw);
        nav.receiveFusedReadings(fused);

        // make decision
        auto decision = nav.makeDecision(car.getPosition());
        Direction moveDir = decision.first;
        string action = decision.second;

        // execute
        outOfBounds = car.executeMovement(moveDir, action, world);

        // Advance world a second time so moving objects and lights step after car if desired
        world.updateTick();
    }

    if (outOfBounds) cout << "Self-driving car attempted to leave the world. Simulation ended.\n";

    cout << "Final world (full):\n";
    world.printFull('@', &car.getPosition());

    return 0;
}