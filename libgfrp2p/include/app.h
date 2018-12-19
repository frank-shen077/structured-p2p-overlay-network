#ifndef APP_H
#define APP_H

#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "peer_manager.h"
#include "node.h"
#include "node_table.h"
#include "param_constants.h"

class BaseApp {
private:
    Node node;
    NodeTable node_table;
    PeerManager peer_manager;

public:
	// constructor
    BaseApp(std::string ip, unsigned short port, std::string id);
    BaseApp(std::string ip, unsigned short port, std::string id, 
    	int num_nodes_in_dist, int num_cnodes_in_dist, 
    	int num_nodes_in_city, int num_cnodes_in_city, 
    	int num_nodes_in_state, int num_cnodes_in_state, 
    	int num_nodes_in_country, int num_cnodes_in_country, 
    	int num_nodes_in_continent);

    // form the network topology
    void form_structure(int num_nodes_in_dist, int num_cnodes_in_dist, 
    	int num_nodes_in_city, int num_cnodes_in_city, 
    	int num_nodes_in_state, int num_cnodes_in_state, 
    	int num_nodes_in_country, int num_cnodes_in_country, 
    	int num_nodes_in_continent);

    // start the application
    void start();

    // stop the application
    void stop();
};

#endif