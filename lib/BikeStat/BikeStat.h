#ifndef BikeStat_h
#define BikeStat_h

class BikeStat {
public:
    // Public variables
    unsigned long bikeTime;
    int bikeLevel;
    int bikePower;
    int bikeCadence;
    int bikeRevs;
    int bikeResistance;
    static String bikeRikeVersion;

    // System parameters
    bool okWiFi;
    bool okMQTT;
    bool openCP;
    String mqttBroker;
    int mqttBrokerPort;

    // Function to get the instance of the singleton class
    static BikeStat& getInstance() {
        // If the instance doesn't exist, create one
        if (!instance) {
            instance = new BikeStat();
        }
        return *instance;
    }

private:
    // Private constructor to prevent direct instantiation
    BikeStat() {
        bikeTime = 0;
        bikeLevel = 0;
        bikePower = 0;
        bikeCadence = 0;
        bikeRevs = 0;
        bikeResistance = 0;

        okWiFi = false;
        okMQTT = false;
        openCP = false;
    }

    // Private destructor to prevent deletion of the instance
    ~BikeStat() {
    }

    // Private copy constructor and assignment operator to prevent copying
    BikeStat(const BikeStat&) = delete;
    BikeStat& operator=(const BikeStat&) = delete;

    // Private static member to hold the instance of the class
    static BikeStat* instance;
};

#endif
