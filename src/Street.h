#pragma once 

#include "TrafficObject.h"

class intersection;

class street final : public traffic_object, public std::enable_shared_from_this<street>
{
public:
    street();

    double getLength() const { return length_; }
    void set_in_intersection(const std::shared_ptr<intersection>& in);
    void set_out_intersection(const std::shared_ptr<intersection>& out);
    std::shared_ptr<intersection> get_out_intersection() const { return inter_out_; }
    std::shared_ptr<intersection> get_in_intersection() const { return inter_in_; }
    std::shared_ptr<street> get_shared_this() { return shared_from_this(); }

private:
    double length_;                                      //Length of this street in m
    std::shared_ptr<intersection> inter_in_, inter_out_; //Intersections from which a vehicle can enter (one-way streets is always from 'in' to 'out')
};