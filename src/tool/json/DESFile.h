/**
 * Autbor: Brent Barbachem
 * Date:   December 22, 2018
 *
 * Class to wrap the Qt Json File. This file will read in the json document
 * and parse the information that is specific to our DES tool.
 */

#ifndef DES_TOOL_DESFILE_H
#define DES_TOOL_DESFILE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QFile>
#include <QJsonArray>
#include "../components/Components.h"

enum FILE_TYPE
{
    UNSUPPORTED,
    TXT,
    JSON
};

class DESFile
{
public:
    /**
     * @param filename - name of the file that will be parsed if it is not empty
     */
    DESFile(QString filename);

    /**
     * Destructor for this file
     */
    ~DESFile();



private:

    /**
     * If the filename is set, open the file, read all contents and store the
     * Qt Json Object
     */
    void read();

    /**
     * Search for the name in the object
     * @param name - name to search for in the json document
     * @return QJsonValue, empty value if the document is empty
     */
    QJsonValue getJsonValue(QString name);
    /**
     * Attempt to parse the file if it is a txt file.
     * @param data - data contained in the .txt file
     */
    void parseTXT(QString data);

    /**
     * Attempt to parse the file if is a json file
     * @param data - data contained in the .json file
     */
    void parseJSON(QString data);

    /**
     * Create all events from the list in the json file.
     * @param events - Json object holding the vents
     * @param order - Json object holding the priority of events
     */
    void orderEvents(QJsonValue events, QJsonValue order);

    /**
     * @param a - First event
     * @param b - Second event
     * @return - return true when event_a.time < event_b.time and when event_a.type < event_b.type
     */
    bool compareEvents(const Event &a, const Event &b) ;

    /**
     * Create all servers from the DES File
     * @param servers - Json Value representing the servers (should be a QJsonArray)
     */
    void createServers(QJsonValue servers);

    /**
     * Create all queues from the DES File
     * @param queues - Json Value representing the queues (should be a QJsonArray)
     */
    void createQueues(QJsonValue queues);

    /**
     * Run the simulation given all DES information that we have read in.
     * To avoid any user issues, this function will only be called at the end
     * of read().
     */
    void simulate();

    /**
     * Function to find the minimum container from the list provided. We should be able to
     * pass both of our lists for queues and servers since they are both technically containers.
     * @param vec - vector of containers to find the minimum in
     * @return - index of the container with the minimum capacity, -1 if there is a failure.
     */
    int findMinAvailableContainer(std::vector<Container*> vec);

private:
    QJsonDocument m_doc;

    QString m_filename;

    std::map<EVENT_TYPE , int> m_event_types;

    std::vector<Event> m_events;
    std::vector<Container*> m_servers;
    std::vector<Container*> m_queues;

    FILE_TYPE m_file_type;

};


#endif //DES_TOOL_DESFILE_H
