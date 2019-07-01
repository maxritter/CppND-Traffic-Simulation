#pragma once

#include <utility>

#include "TrafficObject.h"

class street;
class intersection;

class vehicle final : public traffic_object, public std::enable_shared_from_this<vehicle>
{
public:
	vehicle();

	void set_current_street(std::shared_ptr<street> street) { curr_street_ = std::move(street); }
	void set_current_destination(std::shared_ptr<intersection> destination);

	void simulate() override;

	std::shared_ptr<vehicle> get_shared_this() { return shared_from_this(); }

private:
	void drive();

	std::shared_ptr<street> curr_street_;            //Street on which the vehicle is currently on
	std::shared_ptr<intersection> curr_destination_; //Destination to which the vehicle is currently driving
	double pos_street_;                              //Position on current street
	double speed_;                                   //Ego speed in m/s
};
