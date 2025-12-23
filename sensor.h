#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <string>
#include "types.h"
#include <cmath>

using namespace std;

//-----SENSORS-----//
class Sensors 
{
    public:
        string SENS_TYPE;
        unsigned int RANGE;
        unsigned int VIS_RANGE;
        int ACCURACY;

        //Constructor
        Sensors(const string& sens_type, unsigned int range, unsigned int vis_range, int accuracy)
        : SENS_TYPE(sens_type), RANGE(range), VIS_RANGE(vis_range), ACCURACY(accuracy)
        {
            cout << "I just made a sensor\n";
        }
        //Destructor
        virtual ~Sensors()
        {
            cout << "I just destroyed a sensor\n";
        }
        //Pure virtual describe
        virtual void describe_sensor() const = 0;
};

//----SENSOR LIDAR----//
class Lidar : public Sensors
{
    public:
        float distance;
        string object_type_detect;
        float sureness;
    
        //Constructor
        Lidar(const string& sens_type, unsigned int range, unsigned int vis_range, int accuracy,
        float dist, const string& detect_object, float sure)
        : Sensors(sens_type, range, vis_range, accuracy), distance(dist), object_type_detect(detect_object), sureness(sure)
        {
            cout << "One working lidar sensor\n";
            //Find distance
        }
        //Destructor
        ~Lidar() override
        {
            cout << "Lidar sensor stopped operating\n";
        }
        void describe_sensor() const override
        {
            cout << "Sensor is of type: " << SENS_TYPE << endl;
        }
};

//----SENSOR RADAR----//
class Radar : public Sensors
{
    public:
        float distance;
        //SPEED = STOPPED, HALF_SPEED, FULL_SPEED
        string speed;
        Direction movement_direction;
        float sureness;
    
        //Constructor
        Radar(const string& sens_type, unsigned int range, unsigned int vis_range, int accuracy,
        float dist, const string& sp, Direction dir, float sure)
        : Sensors(sens_type, range, vis_range, accuracy), distance(dist), speed(sp), movement_direction(dir), sureness(sure)
        {
            cout << "One working radar sensor\n";
            //Find distance
        }
        //Destructor
        ~Radar() override
        {
            cout << "Radar sensor stopped operating\n";
        }
        //Describe
        void describe_sensor() const override
        {
            cout << "This sensor is of type: " << SENS_TYPE << endl;
        }
};

//----CAMERA SENSOR----//
class Camera : public Sensors
{
    public:
        string object_type_detect;
        //position of object
        Position position;
        string ObjectID;
        float sureness;
        //SPEED = STOPPED, HALF_SPEED, FULL_SPEED
        string speed;
        //position of car
        Direction movement_direction;
        //Signs can be STOP, YIELD etc
        string SignText;
        //GREEN, YELLOW, RED    
        string LightColour;

        //Constructor
        Camera(const string& sens_type, unsigned int range, unsigned int vis_range, int accuracy,
        const string& detect_object, Position pos, const string& obj_id, float sure, const string& sp,
        Direction dir, const string& signtext, const string& lightcolour)
        : Sensors(sens_type, range, vis_range, accuracy), object_type_detect(detect_object),position(pos), ObjectID(obj_id),
        sureness(sure), speed(sp), movement_direction(dir), SignText(signtext), LightColour(lightcolour)
        {
            cout << "One camera sensor working\n";
            
        }       
        //Destructor
        ~Camera() override
        {
            cout << "Camera sensor stopped working\n";
        }
        //distance from object
        int distance(Position position, Direction movement_direction) const
        {
            return (abs(position.x - movement_direction.x) + (abs(position.y - movement_direction.y)));
        }
        //distance from gps target
        int gps_distance(Position gps_pos, Direction movement_direction) const
        {
            return (abs(gps_pos.x - movement_direction.x) + (abs(gps_pos.y - movement_direction.y)));
        }
        //Describe
        void describe_sensor() const override
        {
            cout << "Sensor is of type: " << SENS_TYPE << endl;
        }
};

#endif // SENSOR_H