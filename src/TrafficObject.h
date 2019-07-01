#pragma once

#include <vector>
#include <thread>
#include <mutex>

enum object_type
{
    no_object,
    object_vehicle,
    object_intersection,
    object_street,
};

class traffic_object
{
public:
    traffic_object();
    virtual ~traffic_object();

    int get_id() const { return id_; }
    void set_position(double x, double y);
    void get_position(double &x, double &y) const;
    object_type get_type() const { return type_; }

    virtual void simulate(){}

protected:
    object_type type_;                 //Identifies the class type
    int id_;                           //Every traffic object has its own unique id
    double pos_x_, pos_y_;             //Vehicle position in pixels
    std::vector<std::thread> threads_; //Holds all threads that have been launched within this object
    static std::mutex mtx_;            //Mutex shared by all traffic objects for protecting cout 

private:
    static int id_cnt_;                //Global variable for counting object ids
};