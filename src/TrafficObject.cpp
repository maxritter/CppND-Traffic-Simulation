#include <algorithm>
#include "TrafficObject.h"

/* Init static variable */
int traffic_object::id_cnt_ = 0;

std::mutex traffic_object::mtx_;

void traffic_object::set_position(const double x, const double y)
{
    pos_x_ = x;
    pos_y_ = y;
}

void traffic_object::get_position(double &x, double &y) const
{
    x = pos_x_;
    y = pos_y_;
}

traffic_object::traffic_object(): pos_x_(0), pos_y_(0)
{
	type_ = no_object;
	id_ = id_cnt_++;
}

traffic_object::~traffic_object()
{
    /* Set up thread barrier before this object is destroyed */
    std::for_each(threads_.begin(), threads_.end(), [](std::thread &t) {
        t.join();
    });
}
