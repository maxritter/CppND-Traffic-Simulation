#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <random>

#include "Street.h"
#include "Intersection.h"
#include "Vehicle.h"

/* Implementation of class "WaitingVehicles" */
int waiting_vehicles::get_size()
{
	std::unique_lock<std::mutex> lck(mutex_);

	return vehicles_.size();
}

void waiting_vehicles::push_back(const std::shared_ptr<vehicle>& vehicle, std::promise<void>&& promise)
{
	std::unique_lock<std::mutex> lck(mutex_);

	vehicles_.push_back(vehicle);
	promises_.push_back(std::move(promise));
}

void waiting_vehicles::permit_entry_to_first_in_queue()
{
	std::unique_lock<std::mutex> lck(mutex_);

	/* Get entries from the front of both queues */
	auto first_promise = promises_.begin();
	auto first_vehicle = vehicles_.begin();

	/* Fulfill promise and send signal back that permission to enter has been granted */
	first_promise->set_value();

	/* Remove front elements from both queues */
	vehicles_.erase(first_vehicle);
	promises_.erase(first_promise);
}

/* Implementation of class "Intersection" */
intersection::intersection()
{
	type_ = object_intersection;
	is_blocked_ = false;
}

void intersection::add_street(const std::shared_ptr<street>& street)
{
	streets_.push_back(street);
}

std::vector<std::shared_ptr<street>> intersection::query_streets(const std::shared_ptr<street>& incoming)
{
	/* Store all outgoing streets in a vector */
	std::vector<std::shared_ptr<street>> outgoings;
	for (const auto& it : streets_)
	{
		if (incoming->get_id() != it->get_id()) //Except the street making the inquiry
		{
			outgoings.push_back(it);
		}
	}

	return outgoings;
}

/* Adds a new vehicle to the queue and returns once the vehicle is allowed to enter */
void intersection::add_vehicle_to_queue(const std::shared_ptr<vehicle>& vehicle)
{
	std::unique_lock<std::mutex> lck(mtx_);
	std::cout << "Intersection #" << id_ << "::addVehicleToQueue: thread id = " << std::this_thread::get_id() << std::endl;
	lck.unlock();

	/* Add new vehicle to the end of the waiting line */
	std::promise<void> prms_vehicle_allowed_to_enter;
	std::future<void> ftr_vehicle_allowed_to_enter = prms_vehicle_allowed_to_enter.get_future();
	waiting_vehicles_.push_back(vehicle, std::move(prms_vehicle_allowed_to_enter));

	/* Wait until the vehicle is allowed to enter */
	ftr_vehicle_allowed_to_enter.wait();
	lck.lock();
	std::cout << "Intersection #" << id_ << ": Vehicle #" << vehicle->get_id() << " is granted entry." << std::endl;
	lck.unlock();

	/* If traffic sign is red, wait until the traffic light turns green */
	if(traffic_light_.get_current_phase() == red)
	{
		traffic_light_.wait_for_green();
	}
}

void intersection::vehicle_has_left(const std::shared_ptr<vehicle>& vehicle)
{
	std::cout << "Intersection #" << id_ << ": Vehicle #" << vehicle->get_id() << " has left." << std::endl;

	/* Unblock queue processing */
	this->set_is_blocked(false);
}

void intersection::set_is_blocked(const bool is_blocked)
{
	is_blocked_ = is_blocked;

	std::cout << "Intersection #" << id_ << " isBlocked=" << is_blocked << std::endl;
}

/* Virtual function which is executed in a thread */
void intersection::simulate() // using threads + promises/futures + exceptions
{
	/* Start the simulation of our traffic light */
	traffic_light_.simulate();

	/* Launch vehicle queue processing in a thread */
	threads_.emplace_back(std::thread(&intersection::process_vehicle_queue, this));
}

void intersection::process_vehicle_queue()
{
	/* Print id of the current thread */
	std::cout << "Intersection #" << id_ << "::processVehicleQueue: thread id = " << std::this_thread::get_id() << std::endl;

	/* Continuously process the vehicle queue */
	while (true)
	{
		/* Sleep at every iteration to reduce CPU usage */
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		/* Only proceed when at least one vehicle is waiting in the queue */
		if (waiting_vehicles_.get_size() > 0 && !is_blocked_)
		{
			/* Set intersection to "blocked" to prevent other vehicles from entering */
			this->set_is_blocked(true);

			/* Permit entry to first vehicle in the queue (FIFO) */
			waiting_vehicles_.permit_entry_to_first_in_queue();
		}
	}
}

bool intersection::traffic_light_is_green() const
{
	return traffic_light_.get_current_phase() == green;
}