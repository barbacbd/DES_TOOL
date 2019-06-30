/**
 * Author: Brent Barbachem
 * Date:   6/28/19.
 */

#include "Simulation.h"
#include <iostream>


Simulation::Simulation(std::vector<Event> events,
        std::vector<DESQueue> queues,
        std::vector<DESServer> servers,
        std::map<EVENT_TYPE , int> event_types)
        : m_events(events), m_queues(queues), m_servers(servers), m_event_types(event_types)
{

}

Simulation::~Simulation()
{

}

void Simulation::run()
{
        if(m_servers.empty() || m_events.empty() || m_queues.empty())
        {
                /// don't try to simulate if there is missing information
                return;
        }

        m_records.clear();

        /// create Records that can be used as a step in time that will
        /// show queue and server state information
        for( auto& event : m_events)
        {

                bool end_simulation = false;

                switch(event.getType())
                {
                        case ARRIVAL:
                        {
//                                std::cout << event.toString() << std::endl;

                                int min_avail_server = findMinAvailableServer(m_servers);
                                if(min_avail_server >= 0)
                                {
                                        /// send the event straight to the server
                                        m_servers[min_avail_server].addEvent(event);
                                }
                                else
                                {
                                        /// find the minimum queue
                                        int min_avail_queue = findMinAvailableQueue(m_queues);
                                        if(min_avail_queue >= 0)
                                        {
                                                m_queues[min_avail_queue].addEvent(event);
                                        } else { continue; }
                                }


                        }
                        break;

                        case DEPARTURE:
                        {
//                                std::cout << event.toString() << std::endl;

                            removeEvent(event);
                            queueToServer();
//
//                                if(removeEvent(event))
//                                {
//                                    queueToServer();
//                                }
                        }
                        break;

                        case TERMINATE:
                        {
//                                std::cout << event.toString() << std::endl;
                                end_simulation = true;
                        }
                        break;
                }

                /// create the record that can be saved as a snapshot
                Record r;
                r.setTime(event.getTime());
                r.setQueues(m_queues);
                r.setServers(m_servers);

                /// add the record to our list
                m_records.push_back(r);

                /// time to end the creation of records ... which means the end of simulation !
                if(end_simulation)
                {
                        break;
                }

        }

//    for(auto &r : m_records)
//    {
//        std::cout << r.toString() << std::endl;
//    }
}

int Simulation::findMinAvailableQueue(std::vector<DESQueue> vec)
{
        int min_size = std::numeric_limits<int>::max();

        int pos = 0;
        int min_pos = -1;
        for(auto &q : vec)
        {
                uint64_t size = q.getEvents().size();

                if(size < min_size && size < q.getCapacity())
                {
                        min_pos = pos;
                        min_size = size;
                }
                pos ++;
        }

        std::cout << "MIN QUEUE = " << min_pos << std::endl;

        return min_pos;
}

int Simulation::findMaxQueue(std::vector<DESQueue> vec)
{
    int max_size = -1;

    int pos = 0;
    int max_pos = -1;
    for(auto &q : vec)
    {
        uint64_t size = q.getEvents().size();

        if(size > max_size)
        {
            max_pos = pos;
            max_size = size;
        }
        pos ++;
    }

    return max_pos;
}

int Simulation::findMinAvailableServer(std::vector<DESServer> vec)
{
    int min_size = std::numeric_limits<int>::max();

    int pos = 0;
    int min_pos = -1;
    for(auto &s : vec)
    {
        uint64_t size = s.getEvents().size();

        if(size < min_size && size < s.getCapacity())
        {
            min_pos = pos;
            min_size = size;
        }
        pos ++;
    }

    std::cout << "MIN SERVER = " << min_pos << std::endl;

    return min_pos;
}

int Simulation::findTermination()
{
        int pos = 0;
        for(auto &e : m_events)
        {
                if(e.getType() == TERMINATE)
                {
                        return pos;
                }
                pos ++;
        }

        return -1;
}

std::map<EVENT_TYPE, Event> Simulation::getNextEventList(int cpos)
{
        bool arrival = false;
        bool departure = false;
        bool termination = false;

        std::map<EVENT_TYPE, Event> next_events;

        int pos = 0;
        for(const Event &event : m_events)
        {

            switch(event.getType())
            {
                case ARRIVAL:
                {
                    if(!arrival)
                    {
                        next_events[ARRIVAL] = Event(event);
                        arrival = true;
                    }
                } break;

                case DEPARTURE:
                {
                    if(!departure)
                    {
                        next_events[DEPARTURE] = Event(event);
                        departure = true;
                    }
                } break;

                case TERMINATE:
                {
                    if(!termination)
                    {
                        next_events[TERMINATE] = Event(event);
                        termination = true;
                    }
                } break;
            }

            pos ++;
        }

        return next_events;
}


bool Simulation::removeEvent(Event &e)
{
        for(auto &server : m_servers)
        {
                if(server.removeEvent(e))
                {
                    std::cout << "Removing event!" << std::endl;
                        return true;
                }
        }

        return false;
}

void Simulation::queueToServer()
{
    /// while the servers are not full ... and there are still elements in the queues ...
    /// add the next element to leave the [max] queue to the min server

    int min_server;

    while((min_server = findMinAvailableServer(m_servers) >= 0))
    {
        int max_pos = findMaxQueue(m_queues);

        if(max_pos < 0 )
        {
            /// no more elements in the queues
            break;
        }

        /// add the element from the queue to the min server
        Event event = m_queues[max_pos].getNextExit();
        m_servers[min_server].addEvent(event);
        m_queues[max_pos].dequeue();
    }
}