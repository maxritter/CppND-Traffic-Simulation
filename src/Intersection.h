#pragma once

#include <vector>
#include <future>
#include <mutex>
#include <memory>

#include "TrafficObject.h"
#include "TrafficLight.h"

class street;
class vehicle;

/* Auxiliary class to queue and dequeue waiting vehicles in a thread-safe manner */
class waiting_vehicles
{
public:
    int get_size();

    void push_back(const std::shared_ptr<vehicle>& vehicle, std::promise<void> &&promise);
    void permit_entry_to_first_in_queue();

private:
    std::vector<std::shared_ptr<vehicle>> vehicles_;  //List of all vehicles waiting to enter this intersection
    std::vector<std::promise<void>> promises_;        //List of associated promises
    std::mutex mutex_;
};

class intersection final : public traffic_object
{
public:
    intersection();

    void set_is_blocked(bool is_blocked);

    void add_vehicle_to_queue(const std::shared_ptr<vehicle>& vehicle);
    void add_street(const std::shared_ptr<street>& street);
    std::vector<std::shared_ptr<street>> query_streets(const std::shared_ptr<street>& incoming); //Return pointer to current list of all outgoing streets
    void simulate() override;
    void vehicle_has_left(const std::shared_ptr<vehicle>& vehicle);
    bool traffic_light_is_green() const;

private:
    void process_vehicle_queue();

	traffic_light traffic_light_;
    std::vector<std::shared_ptr<street>> streets_;   //List of all streets connected to this intersection
    waiting_vehicles waiting_vehicles_;              //List of all vehicles and their associated promises waiting to enter the intersection
    bool is_blocked_;                                //Flag indicating wether the intersection is blocked by a vehicle
};
