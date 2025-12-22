int main(int argc, char* argv[])
{
    using namespace std;

    // Defaults
    unsigned long long seed = static_cast<unsigned long long>(
        std::chrono::system_clock::now().time_since_epoch().count());
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
    std::mt19937_64 rng(seed);

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

    // At this point the world and vehicle would be created and simulation started.
    return 0;
}
