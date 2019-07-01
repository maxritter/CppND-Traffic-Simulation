#pragma once
#include <mutex>
#include <deque>
#include <condition_variable>

#include "TrafficObject.h"

class vehicle;

enum traffic_light_phase
{
	red,
	green,
};

template <class T>
class message_queue
{
public:
	T receive();
	void send(T&& msg);

private:
	std::mutex mutex_;
	std::condition_variable cond_;
	std::deque<T> queue_;
};

class traffic_light final : public traffic_object
{
public:
	traffic_light();

	traffic_light_phase get_current_phase() const;

	void wait_for_green() const;
	void simulate() override;

private:
	void cycle_through_phases();

	std::shared_ptr<message_queue<traffic_light_phase>> msg_queue_;
	traffic_light_phase current_phase_;
	std::condition_variable condition_;
	std::mutex mutex_;
};