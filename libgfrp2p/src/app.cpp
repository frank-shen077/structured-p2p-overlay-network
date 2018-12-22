#include "app.h"

// constructors
BaseApp::BaseApp(std::string ip, unsigned short port, std::string id) {
    this->node = std::make_shared<Node>(id, ip, port);
    this->node_table = std::make_shared<NodeTable>(id);
    // this->peer_manager = PeerManager(node, node_table);
}

// public functions
void BaseApp::form_structure(int num_nodes_in_dist, int num_cnodes_in_dist, 
        int num_nodes_in_city, int num_cnodes_in_city, 
        int num_nodes_in_state, int num_cnodes_in_state, 
        int num_nodes_in_country, int num_cnodes_in_country, 
        int num_nodes_in_continent, unsigned short starting_port_number) {
    // form network topology based ID
    std::string id_in_dist = this->node->get_id().substr(ID_SINGLE_START, ID_SINGLE_LEN);
    std::string dist_id = this->node->get_id().substr(ID_DISTRICT_START, ID_DISTRICT_LEN);
    std::string city_id = this->node->get_id().substr(ID_CITY_START, ID_CITY_LEN);
    std::string state_id = this->node->get_id().substr(ID_STATE_START, ID_STATE_LEN);
    std::string country_id = this->node->get_id().substr(ID_COUNTRY_START, ID_COUNTRY_LEN);
    std::string continent_id = this->node->get_id().substr(ID_CONTINENT_START, ID_CONTINENT_LEN);

    std::stringstream ss_node(id_in_dist);
    int node_id_in_dist = 0;
    ss_node >> node_id_in_dist;

    int num_dists_in_city = num_nodes_in_city/num_cnodes_in_dist;
    int num_cities_in_state = num_nodes_in_state/num_cnodes_in_dist;

    std::vector<Ring> tables;
    std::unordered_map<std::string, std::shared_ptr<Node>> contact_nodes;  // contact nodes of the ring
    std::shared_ptr<Node> predecessor;                                     // successor within the ring
    std::shared_ptr<Node> successor;                                       // predecessor within the ring
    std::unordered_map<std::string, std::shared_ptr<Node>> peer_set;       // peers in an unordered map
    std::vector<std::shared_ptr<Node>> peer_list;                          // used for broadcast within ring

    // used locally for structure establishment
    std::unordered_map<std::string, std::shared_ptr<Node>> contact_nodes_next; 

    // normal nodes - peer level
    Ring table_peer;
    table_peer.ring_level = 0;

    // add other peers
    std::stringstream ss;
    for (int i = 0; i < num_nodes_in_dist; i++) {
        ss.str("");
        ss.clear();
        ss << std::setw(9) << std::setfill('0') << i;
        std::string peer_id_in_dist = ss.str();
        std::string node_id = this->node->get_id().substr(0, ID_SINGLE_START) + peer_id_in_dist;
        unsigned short port = this->convert_ID_to_port(starting_port_number, node_id,
                            num_nodes_in_dist, num_cnodes_in_dist, 
                            num_nodes_in_city, num_cnodes_in_city, 
                            num_nodes_in_state, num_cnodes_in_state, 
                            num_nodes_in_country, num_cnodes_in_country, 
                            num_nodes_in_continent);
        Node node(node_id, "127.0.0.1", port);

        // insert into contact node list
        if (i < num_cnodes_in_dist) {
            contact_nodes.insert({node_id, std::make_shared<Node>(node)});
        }

        // check predecessor or successor
        if (node_id_in_dist == 0 && i == num_nodes_in_dist - 1) {
            predecessor = std::make_shared<Node>(node);
        } else if (node_id_in_dist == num_nodes_in_dist - 1 && i == 0) {
            successor = std::make_shared<Node>(node);
        } else if (i == node_id_in_dist + 1) {
            successor = std::make_shared<Node>(node);
        } else if (i == node_id_in_dist - 1) {
            predecessor = std::make_shared<Node>(node);
        }

        // insert into peer list
        peer_set.insert({node_id, std::make_shared<Node>(node)});
        peer_list.push_back(std::make_shared<Node>(node));
    }

    table_peer.contact_nodes = contact_nodes;
    table_peer.predecessor = predecessor;
    table_peer.successor = successor;
    table_peer.peer_set = peer_set;
    table_peer.peer_list = peer_list;

    tables.push_back(table_peer);
    
    // contact nodes - dist level
    if (node_id_in_dist < num_cnodes_in_dist) {
        // should be the contact node of the dist level ring
        Ring table_dist;
        table_dist.ring_level = 1;

        // reset stringstream and ring variables
        contact_nodes.clear();
        peer_set.clear();
        peer_list.clear();

        // peers should be the contact node of level one, which form a dist
        // contact nodes at this level should be the nodes with node_id < num_cnodes_in_city/num_dists_in_city

        // add other peers in this level
        for (int i = 0; i < num_dists_in_city; i++) {
            std::string node_id = this->node->get_id().substr(0, ID_DISTRICT_START);
            ss.str("");
            ss.clear();
            ss << std::setw(5) << std::setfill('0') << i;
            std::string dist_id_in_city = ss.str();
            node_id += dist_id_in_city;
            for (int j = 0; j < num_cnodes_in_dist; j++) {
                ss.str("");
                ss.clear();
                ss << std::setw(9) << std::setfill('0') << j;
                std::string peer_id_in_dist = ss.str();
                std::string peer_id = node_id + peer_id_in_dist;
                unsigned short port = this->convert_ID_to_port(starting_port_number, peer_id,
                    num_nodes_in_dist, num_cnodes_in_dist, 
                    num_nodes_in_city, num_cnodes_in_city, 
                    num_nodes_in_state, num_cnodes_in_state, 
                    num_nodes_in_country, num_cnodes_in_country, 
                    num_nodes_in_continent);
                Node node(peer_id, "127.0.0.1", port);

                // insert into peer list
                // BOOST_LOG_TRIVIAL(debug) << i << " " << j << " " << peer_id << "(" << peer_id.length() << ")" << " added to peer_set";
                peer_set.insert({peer_id, std::make_shared<Node>(node)});
                peer_list.push_back(std::make_shared<Node>(node));

                // predecessor & successor (No Need?)

                // contact nodes
                if (j < num_cnodes_in_city/num_dists_in_city) {
                    contact_nodes.insert({peer_id, std::make_shared<Node>(node)});
                }
            }
        }

        table_dist.contact_nodes = contact_nodes;
        table_dist.predecessor = NULL;
        table_dist.successor = NULL;
        table_dist.peer_set = peer_set;
        table_dist.peer_list = peer_list;

        tables.push_back(table_dist);
    }

    // contact nodes - city level
    if (node_id_in_dist < num_cnodes_in_city/num_dists_in_city) {
        // should be the contact node of the city level ring
        Ring table_city;
        table_city.ring_level = 2;

        // reset stringstream and ring variables
        contact_nodes.clear();
        peer_set.clear();
        peer_list.clear();

        // peers should be the contact node of level two, which form a city
        // contact nodes at this level should be the nodes with node_id < num_cnodes_in_state/num_cities_in_state

        // add other peers in that level
        for (int i = 0; i < num_cities_in_state; i++) {
            std::string node_id = this->node->get_id().substr(0, ID_CITY_START);
            ss.str("");
            ss.clear();
            ss << std::setw(6) << std::setfill('0') << i;
            std::string city_id_in_state = ss.str();
            node_id += city_id_in_state;

            for (int j = 0; j < num_dists_in_city; j++) {
                ss.str("");
                ss.clear();
                ss << std::setw(5) << std::setfill('0') << j;
                std::string dist_id_in_city = ss.str();
                node_id += dist_id_in_city;
                for (int k = 0; k < num_cnodes_in_dist; k++) {
                    if (k < num_cnodes_in_city/num_dists_in_city) {
                        ss.str("");
                        ss.clear();
                        ss << std::setw(9) << std::setfill('0') << k;
                        std::string peer_id_in_dist = ss.str();
                        node_id += peer_id_in_dist;
                        unsigned short port = this->convert_ID_to_port(starting_port_number, node_id,
                            num_nodes_in_dist, num_cnodes_in_dist, 
                            num_nodes_in_city, num_cnodes_in_city, 
                            num_nodes_in_state, num_cnodes_in_state, 
                            num_nodes_in_country, num_cnodes_in_country, 
                            num_nodes_in_continent);
                        Node node(node_id, "127.0.0.1", port);

                        // insert into peer list
                        peer_set.insert({node_id, std::make_shared<Node>(node)});
                        peer_list.push_back(std::make_shared<Node>(node));

                        // predecessor & successor (No Need?)

                        // contact nodes
                        if (j < num_cnodes_in_state/num_cities_in_state) {
                            contact_nodes.insert({node_id, std::make_shared<Node>(node)});
                        }

                        // contact nodes for the next level
                        if (j == 0 && k == 0) {
                            contact_nodes_next.insert({node_id, std::make_shared<Node>(node)});
                        }
                        node_id = node_id.substr(0, ID_SINGLE_START);
                    }
                }
                node_id = node_id.substr(0, ID_DISTRICT_START);
            }
            node_id = node_id.substr(0, ID_CITY_START);
        }

        table_city.contact_nodes = contact_nodes;
        table_city.predecessor = NULL;
        table_city.successor = NULL;
        table_city.peer_set = peer_set;
        table_city.peer_list = peer_list;

        tables.push_back(table_city);
    }

    // contact nodes - state level [currently the top level]
    /*
    if (node_id_in_dist < num_cnodes_in_state/num_cities_in_state) {
        // should be the contact node of the state level ring
        Ring table_state;
        table_state.ring_level = 3;

        // reset stringstream and ring variables
        peer_set.clear();
        peer_list.clear();
        ss.str("");
        ss.clear();

        // peers should be the contact node of level three, which form a state
        for (auto contact_node : contact_nodes) {
            peer_set.insert({contact_node.first, contact_node.second});
            peer_list.push_back(contact_node.second);
        }

        // contact nodes are set during the above stage
        contact_nodes.clear();
        contact_nodes = contact_nodes_next;

        table_state.contact_nodes = contact_nodes;
        table_state.predecessor = NULL;
        table_state.successor = NULL;
        table_state.peer_set = peer_set;
        table_state.peer_list = peer_list;

        tables.push_back(table_state);
    }*/

    this->node_table->set_tables(tables);

    // [TODO]
    // contact nodes - country level
    // contact nodes - continent level
}

// convert ID to port
unsigned short BaseApp::convert_ID_to_port(unsigned short starting_port_number, const std::string& id,
    int num_nodes_in_dist, int num_cnodes_in_dist, 
    int num_nodes_in_city, int num_cnodes_in_city, 
    int num_nodes_in_state, int num_cnodes_in_state, 
    int num_nodes_in_country, int num_cnodes_in_country, 
    int num_nodes_in_continent) {
    // BOOST_LOG_TRIVIAL(debug) << id;
    
    std::string id_in_dist = id.substr(ID_SINGLE_START);
    std::string dist_id = id.substr(ID_DISTRICT_START, ID_DISTRICT_LEN);
    std::string city_id = id.substr(ID_CITY_START, ID_CITY_LEN);
    std::string state_id = id.substr(ID_STATE_START, ID_STATE_LEN);
    std::string country_id = id.substr(ID_COUNTRY_START, ID_COUNTRY_LEN);
    std::string continent_id = id.substr(ID_CONTINENT_START, ID_CONTINENT_LEN);

    int node_id_in_dist = 0, dist_id_int = 0, city_id_int = 0, state_id_int = 0, country_id_int = 0, continent_id_int = 0;
    
    std::stringstream ss_node(id_in_dist);
    ss_node >> node_id_in_dist;

    std::stringstream ss_dist(dist_id);
    ss_dist >> dist_id_int;
    int num_nodes_in_one_dist = num_nodes_in_dist;

    int num_dists_in_city = num_nodes_in_city/num_cnodes_in_dist;
    std::stringstream ss_city(city_id);
    ss_city >> city_id_int;
    int num_nodes_in_one_city = num_nodes_in_one_dist * num_dists_in_city;

    int num_cities_in_state = num_nodes_in_state/num_cnodes_in_dist;
    std::stringstream ss_state(state_id);
    ss_state >> state_id_int;
    int num_nodes_in_one_state = num_nodes_in_one_city * num_cities_in_state;

    int num_states_in_country = num_nodes_in_country/num_cnodes_in_state;
    std::stringstream ss_country(country_id);
    ss_country >> country_id_int;
    int num_nodes_in_one_country = num_nodes_in_one_state * num_states_in_country;

    int num_countries_in_continent = num_nodes_in_continent/num_cnodes_in_country;
    std::stringstream ss_continent(continent_id);
    ss_continent >> continent_id_int;
    int num_nodes_in_one_continent = num_nodes_in_one_country * num_countries_in_continent;

    int port_num = starting_port_number + 
                   num_nodes_in_one_continent * continent_id_int +
                   num_nodes_in_one_country * country_id_int +
                   num_nodes_in_one_state * state_id_int +
                   num_nodes_in_one_city * city_id_int +
                   num_nodes_in_one_dist * dist_id_int +
                   node_id_in_dist;
    /*BOOST_LOG_TRIVIAL(debug) << "port num = " << starting_port_number << "+"
                   << num_nodes_in_one_continent << "*" << continent_id_int << "+"
                   << num_nodes_in_one_country << "*" << country_id_int << "+"
                   << num_nodes_in_one_state << "*" << state_id_int << "+"
                   << num_nodes_in_one_city << "*" << city_id_int << "+"
                   << num_nodes_in_one_dist << "*"  << dist_id_int << "+"
                   << node_id_in_dist << " = " << port_num;*/

    return port_num;
}

void BaseApp::start(const std::string &start_time, int num_nodes_in_dist, int num_cnodes_in_dist, 
        int num_nodes_in_city, int num_cnodes_in_city, 
        int num_nodes_in_state, int num_cnodes_in_state, 
        int num_nodes_in_country, int num_cnodes_in_country, 
        int num_nodes_in_continent, unsigned short starting_port_number) {
    BOOST_LOG_TRIVIAL(debug) << "Setting up NodeTable for node [ID: " + this->node->get_id() + "] [IP: " + this->node->get_ip() + "] [" + std::to_string(this->node->get_port()) + "]";
    BOOST_LOG_TRIVIAL(debug) << "Establishing structure on node [ID: " + this->node->get_id() + "] [IP: " + this->node->get_ip() + "] [" + std::to_string(this->node->get_port()) + "]";
    this->form_structure(num_nodes_in_dist, num_cnodes_in_dist, 
        num_nodes_in_city, num_cnodes_in_city, 
        num_nodes_in_state, num_cnodes_in_state, 
        num_nodes_in_country, num_cnodes_in_country, 
        num_nodes_in_continent, starting_port_number);
    BOOST_LOG_TRIVIAL(debug) << "Structure established on node [ID: " + this->node->get_id() + "] [IP: " + this->node->get_ip() + "] [" + std::to_string(this->node->get_port()) + "]";
    BOOST_LOG_TRIVIAL(debug) << "Node Tables on node [ID: " + this->node->get_id() + "] [IP: " + this->node->get_ip() + "] [" + std::to_string(this->node->get_port()) + "]";
    for (auto table : this->node_table->get_tables()) {
        BOOST_LOG_TRIVIAL(debug) << "Level: " + std::to_string(table.ring_level);
        for (auto peer : table.peer_set) {
            BOOST_LOG_TRIVIAL(debug) << "Peer - " + peer.first + " " + peer.second->get_ip() + ":" + std::to_string(peer.second->get_port());
        }
        for (auto contact_node : table.contact_nodes) {
            BOOST_LOG_TRIVIAL(debug) << "Contact node - " + contact_node.first + " " + contact_node.second->get_ip() + ":" + std::to_string(contact_node.second->get_port());
        }
    }

    this->peer_manager = std::shared_ptr<PeerManager>(node, node_table, start_time);

    BOOST_LOG_TRIVIAL(debug) << "Starting HGFR PeerManager on node [ID: " + this->node->get_id() + "] [IP: " + this->node->get_ip() + "] [" + std::to_string(this->node->get_port()) + "]";
    this->peer_manager->start();
}

void BaseApp::stop() {
    // BOOST_LOG_TRIVIAL(debug) << "Stopping HGFR PeerManager on node [ID: " + this->node->get_id() + "] [IP: " + this->node->get_ip() + "] [" + std::to_string(this->node->get_port()) + "]";
    this->peer_manager->stop();
}

void BaseApp::broadcast(const std::string &data) {
    this->peer_manager->broadcast(data);
}

int main(int argc, char** argv) {
    if (argc != 15) {
        BOOST_LOG_TRIVIAL(info) << "Wrong arguments. Correct usage: ./app ip_addr port_num id num_nodes_in_dist num_cnodes_in_dist num_nodes_in_city num_cnodes_in_city num_nodes_in_state num_cnodes_in_state num_nodes_in_country num_cnodes_in_country num_nodes_in_continent starting_port_num start_time\n";
        return 0;
    }

    std::string ip = argv[1];
    unsigned short port = (unsigned short) std::atoi(argv[2]);
    std::string id = argv[3];

    // information used for network topology establishment (only used for evaluation)
    int num_nodes_in_dist = std::atoi(argv[4]);
    int num_cnodes_in_dist = std::atoi(argv[5]);
    int num_nodes_in_city = std::atoi(argv[6]);
    int num_cnodes_in_city = std::atoi(argv[7]); 
    int num_nodes_in_state = std::atoi(argv[8]);
    int num_cnodes_in_state = std::atoi(argv[9]); 
    int num_nodes_in_country = std::atoi(argv[10]);
    int num_cnodes_in_country = std::atoi(argv[11]); 
    int num_nodes_in_continent = std::atoi(argv[12]);
    int starting_port_number = std::atoi(argv[13]);
    std::string start_time = argv[14];

    // initialize the app
    BOOST_LOG_TRIVIAL(debug) << "Creating HGFR base application on node [ID: " + id + "] [IP: " + ip + "] [" + std::to_string(port) + "]";
    BaseApp app = BaseApp(ip, port, id);

    // start the app service
    BOOST_LOG_TRIVIAL(debug) << "Starting HGFR base service on node [ID: " + id + "] [IP: " + ip + "] [" + std::to_string(port) + "]";
    app.start(start_time, num_nodes_in_dist, num_cnodes_in_dist, 
        num_nodes_in_city, num_cnodes_in_city, 
        num_nodes_in_state, num_cnodes_in_state,
        num_nodes_in_country, num_cnodes_in_country,
        num_nodes_in_continent, starting_port_number);

    if (id == "00000000000000000000000000000000") {
        std::this_thread::sleep_for (std::chrono::seconds(3));
        BOOST_LOG_TRIVIAL(debug) << "Slept for 3 seconds";
        BOOST_LOG_TRIVIAL(debug) << "Broadcasting message ...";
        app.broadcast("Hello World!");
    }

    // stop the app service
    // BOOST_LOG_TRIVIAL(debug) << "Stopping HGFR base service on node [ID: " + id + "] [IP: " + ip + "] [" + std::to_string(port) + "]";
    app.stop();

    return 0;
}