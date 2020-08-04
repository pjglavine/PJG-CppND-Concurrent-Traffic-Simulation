#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;
enum TrafficLightPhase {red, green}; //FP.1 Create data structure for traffic light phases.

// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
	T receive(); // Function for receiving messages.
	void send(T &&msg); //Function for sending messages.

private:
	std::mutex _mutex; // Mutex for modifying messages under lock.
	std::condition_variable _cond;
	std::deque<T> _messages; 
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject //inherit from TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    // ~TrafficLight(); // Default destructor will do.
    // getters / setters
    
    TrafficLightPhase getCurrentPhase(); // Function for getting the current phase of a TrafficLight object.

    // typical behaviour methods
	void waitForGreen(); // Funtion that waits for the light to change to green.
	void simulate(); // Traffic Light simulation function. Starts TrafficLight threads.

private:
    // typical behaviour methods
    void cycleThroughPhases(); // Function cycles through phases red and green.
    
    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

	MessageQueue<TrafficLightPhase> _messages; //Create MessageQueue member for TrafficLightPhase messages.
	TrafficLightPhase _currentPhase; // Current light color of the traffic light.
    std::condition_variable _condition; // Condition variable for message queue notifications.
    std::mutex _mutex; // Mutex for our locking mechanisms when accessing data with multiple threads.
};

#endif