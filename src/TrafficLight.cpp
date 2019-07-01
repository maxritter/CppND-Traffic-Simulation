#include <iostream>
#include <random>

#include "TrafficLight.h"
#include <queue>
#include <future>

/* Implementation of class "message_queue" */
template <class T>
T message_queue<T>::receive()
{
	/* Create a lock and pass it to the condition variable */
	std::unique_lock<std::mutex> u_lock(mutex_);
	cond_.wait(u_lock, [this] { return !queue_.empty(); });

	/* Get the latest element and remove it from the queue */
	T msg = std::move(queue_.back());
	queue_.pop_back();
	return msg;
}

template <class T>
void message_queue<T>::send(T&& msg)
{
	/* Prevent data race */
	std::lock_guard<std::mutex> u_lock(mutex_);

	/* Move into queue and notify client */
	queue_.push_back(std::move(msg));
	cond_.notify_one();
}

/* Implementation of class "traffic_light" */
traffic_light::traffic_light()
{
	current_phase_ = red;
	msg_queue_ = std::make_shared<message_queue<traffic_light_phase>>();
}

traffic_light_phase traffic_light::get_current_phase() const
{
	return current_phase_;
}

void traffic_light::wait_for_green() const
{
	while (true)
	{
		/* Sleep at every iteration to reduce CPU usage */
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		/* Wait until the traffic light is green, received from message queue */
		auto curr_phase = msg_queue_->receive();
		if (curr_phase == green)
		{
			return;
		}
	}
}

void traffic_light::simulate()
{
	threads_.emplace_back(std::thread(&traffic_light::cycle_through_phases, this));
}

/* Virtual function which is executed in a thread */
void traffic_light::cycle_through_phases()
{
	/* Init our random generation between 4 and 6 seconds */
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(4, 6);

	/* Print id of the current thread */
	std::unique_lock<std::mutex> lck(mtx_);
	std::cout << "Traffic_Light #" << id_ << "::Cycle_Through_Phases: thread id = " << std::this_thread::get_id() << std::endl;
	lck.unlock();

	/* Initalize variables */
	int cycle_duration = distr(eng); //Duration of a single simulation cycle in seconds, is randomly chosen

	/* Init stop watch */
	auto last_update = std::chrono::system_clock::now();
	while (true)
	{
		/* Compute time difference to stop watch */
		long time_since_last_update = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_update).count();

		/* Sleep at every iteration to reduce CPU usage */
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		/* It is time to toggle our traffic light */
		if (time_since_last_update >= cycle_duration)
		{
			/* Toggle current phase of traffic light */
			if (current_phase_ == red)
			{
				current_phase_ = green;
			}
			else
			{
				current_phase_ = red;
			}

			/* Send an update to the message queue and wait for it to be sent */
			auto msg = current_phase_;
			auto is_sent = std::async(std::launch::async, &message_queue<traffic_light_phase>::send, msg_queue_, std::move(msg));
			is_sent.wait();

			/* Reset stop watch for next cycle */
			last_update = std::chrono::system_clock::now();

			/* Randomly choose the cycle duration for the next cycle */
			cycle_duration = distr(eng);
		}
	}
}
