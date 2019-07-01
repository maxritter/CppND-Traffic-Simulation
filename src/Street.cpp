#include "Intersection.h"
#include "Street.h"

street::street()
{
    type_ = object_street;
    length_ = 1000.0; //in m
}

void street::set_in_intersection(const std::shared_ptr<intersection>& in)
{
    inter_in_ = in;
    in->add_street(get_shared_this()); //Add this street to list of streets connected to the intersection
}

void street::set_out_intersection(const std::shared_ptr<intersection>& out)
{
    inter_out_ = out;
    out->add_street(get_shared_this()); //Add this street to list of streets connected to the intersection
}
