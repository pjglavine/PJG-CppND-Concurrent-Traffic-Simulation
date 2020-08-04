#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

  	// Perform work with lock in place.
	std::unique_lock<std::mutex> uLock(_mutex);
	_cond.wait(uLock, [this] { return !_messages.empty(); }); // pass unique lock to condition variable

	// remove last vector element from queue
	T msg = std::move(_messages.back());
	_messages.pop_back();

	return msg; // will not be copied due to return value optimization (RVO) in C++

}


template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

	// perform vector message under the lock
	std::lock_guard<std::mutex> uLock(_mutex);

	// add message to queue
	_messages.push_back(std::move(msg));
	_cond.notify_one(); // notify client after pushing new TrafficLightPhase into Message Queue

}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
  
    // Keep checking if the light has turned green. Return when it does.
	while (true){
		if (_messages.receive() == TrafficLightPhase::green){
			return;
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
	threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this)); // start TrafficLight phase cycling threads.

}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
 
    // Modified this section as requested.
  	std::random_device r; //create random device
 
    std::default_random_engine ran(r()); // build random engine with device.
    std::uniform_int_distribution<int> uniform_dist(4000, 6000); // create uniform distribution of numbers between 4-6 seconds.
    int delayCycle = uniform_dist(ran); // pull a random mean from that distribution for first iteration.
  
    auto first = std::chrono::high_resolution_clock::now(); // this is the time point reference used to measure how much time has passed each loop.
    auto next = std::chrono::high_resolution_clock::now(); // this is the time measurement we take each cycle and compare with the first time.

    while(true){
      next = std::chrono::high_resolution_clock::now(); // update the measured time.
      if (std::chrono::duration_cast<std::chrono::milliseconds>(next - first) > std::chrono::milliseconds(delayCycle)){ // compare the time now to the first time point and see if it's greater than the random delayCycle.
        // update the traffic light, the first time point and generate a new random mean value from the random engine.
        if (_currentPhase == TrafficLightPhase::red){
          _currentPhase = TrafficLightPhase::green;
          first = std::chrono::high_resolution_clock::now();
          delayCycle = uniform_dist(ran);
        } else {
          _currentPhase = TrafficLightPhase::red;
          first = std::chrono::high_resolution_clock::now();
          delayCycle = uniform_dist(ran);
        }
        _messages.send(std::move(_currentPhase)); // Send _currentPhase to the message queue.
        
        //std::this_thread::sleep_for(std::chrono::seconds(std::move(rand() % 2 + 4))); // Sleep for 4-6 seconds. This was the previous method used, removed in this solution.
        
        // if 4-6 seconds haven't passed this loop, sleep for 1 millisecond and check again.
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }

}

