#include <iostream>
#include <random>
#include <utility>

#include "Street.h"
#include "Intersection.h"
#include "Vehicle.h"

vehicle::vehicle()
{
	curr_street_ = nullptr;
	pos_street_ = 0.0;
	type_ = object_vehicle;
	speed_ = 400; //m/s
}

void vehicle::set_current_destination(std::shared_ptr<intersection> destination)
{
	/* Update destination using move semantic */
	curr_destination_ = std::move(destination);

	/* Reset simulation parameters */
	pos_street_ = 0.0;
}

void vehicle::simulate()
{
	/* Launch drive function in a thread using "enable_shared_from_this" */
	threads_.emplace_back(std::thread(&vehicle::drive, this));
}

/* Virtual function which is executed in a thread */
void vehicle::drive()
{
	/* Print id of the current thread */
	std::unique_lock<std::mutex> lck(mtx_);
	std::cout << "Vehicle #" << id_ << "::drive: thread id = " << std::this_thread::get_id() << std::endl;
	lck.unlock();

	/* Initalize variables */
	bool has_entered_intersection = false;
	double cycle_duration = 1; //Duration of a single simulation cycle in ms

	/* Init stop watch */
	auto last_update = std::chrono::system_clock::now();
	while (true)
	{
		/* Sleep at every iteration to reduce CPU usage */
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		/* Compute time difference to stop watch */
		long time_since_last_update = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_update).count();
		if (time_since_last_update >= cycle_duration)
		{
			/* Update position with a constant velocity motion model */
			pos_street_ += speed_ * time_since_last_update / 1000;

			/* Compute completion rate of current street */
			double completion = pos_street_ / curr_street_->getLength();
			auto i2 = curr_destination_;
			auto i1 = i2->get_id() == curr_street_->get_in_intersection()->get_id() ? 
				curr_street_->get_out_intersection() : curr_street_->get_in_intersection();

			double x1, y1, x2, y2;
			i1->get_position(x1, y1);
			i2->get_position(x2, y2);
			double dx = x2 - x1;
			double dy = y2 - y1;
			double xv = x1 + completion * dx; //New position based on line equation in parameter form
			double yv = y1 + completion * dy;
			this->set_position(xv, yv);

			/* Check wether halting position in front of destination has been reached */
			if (completion >= 0.9 && !has_entered_intersection)
			{
				/* Request entry to the current intersection (using async) */
				auto ftr_entry_granted = std::async(&intersection::add_vehicle_to_queue, curr_destination_, get_shared_this());

				/* Wait until entry has been granted */
				ftr_entry_granted.get();

				/* Slow down and set intersection flag */
				speed_ /= 10.0;
				has_entered_intersection = true;
			}

			/* Check wether intersection has been crossed */
			if (completion >= 1.0 && has_entered_intersection)
			{
				/* Choose next street and destination */
				auto street_options = curr_destination_->query_streets(curr_street_);
				std::shared_ptr<street> next_street;
				if (!street_options.empty())
				{
					/* Pick one street at random and query intersection to enter this street */
					std::random_device rd;
					std::mt19937 eng(rd());
					std::uniform_int_distribution<> distr(0, street_options.size() - 1);
					next_street = street_options.at(distr(eng));
				}
				else
				{
					/* This street is a dead-end, so drive back the same way */
					next_street = curr_street_;
				}

				/* Pick the one intersection at which the vehicle is currently not */
				auto next_intersection = next_street->get_in_intersection()->get_id() == curr_destination_->get_id() ? 
					next_street->get_out_intersection() : next_street->get_in_intersection();

				/* Send signal to intersection that vehicle has left the intersection */
				curr_destination_->vehicle_has_left(get_shared_this());

				/* Assign new street and destination */
				this->set_current_destination(next_intersection);
				this->set_current_street(next_street);

				/* Reset speed and intersection flag */
				speed_ *= 10.0;
				has_entered_intersection = false;
			}

			/* Reset stop watch for next cycle */
			last_update = std::chrono::system_clock::now();
		}
	}
}
