#ifndef NODE_TABLE_H
#define NODE_TABLE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>

#include <boost/log/trivial.hpp>

#include "node.h"

// If an id exists in multiple maps, they must be the same object
struct Ring {
    unsigned long ring_level;                                              // level of the ring
    std::unordered_map<std::string, std::shared_ptr<Node>> contact_nodes;  // contact nodes of the ring
    std::shared_ptr<Node> predecessor;                                     // successor within the ring
    std::shared_ptr<Node> successor;                                       // predecessor within the ring
    std::unordered_map<std::string, std::shared_ptr<Node>> peer_list;      // used for broadcast within ring
};

// Node table and maintenance
class NodeTable {
private:
    const std::string self_id;

    // store nodes in level-referenced sets
    std::vector<Ring> tables;

    // thread safty
    std::mutex mlock;

    // copy of pointer, use only when locked
    std::shared_ptr<Node> get_node(const std::string& id);
    std::shared_ptr<Node> copy_node(const std::shared_ptr<Node>& node);
 
public:
    NodeTable(const std::string& self_id);

    // all operations except those involving const members only must get lock

    /* self operations */
    std::string get_self_id() const;

    /* storage operations */
    bool has_node(const std::string& id);
    // deep copy of node information
    std::shared_ptr<Node> get_node_copy(const std::string& id);

    /* thread safe node operations */
    void set_node_last_pong_now(const std::string& id);
    void set_node_last_ping_now(const std::string& id);

    /* domain logic */
    bool is_contact_node(unsigned long level);
    // all nodes returned are deep copy for thread safty
    std::unordered_set<std::shared_ptr<Node>> get_contact_nodes(unsigned long level);
    std::unordered_set<std::shared_ptr<Node>> get_successor(unsigned long level);
    std::unordered_set<std::shared_ptr<Node>> get_predecessor(unsigned long level);
    std::unordered_set<std::shared_ptr<Node>> get_peer_list(unsigned long level);
    // for broadcast in ring (k-ary distributed spanning tree)
    std::shared_ptr<Node> get_peer(unsigned long level, int id);  // get the particular peer
    int get_end_id(unsigned long level);                          // get the end id in the ring
};

#endif