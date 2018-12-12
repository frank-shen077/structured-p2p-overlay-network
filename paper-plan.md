# Hidden Geographical Fractal Ring

### Observation on P2P Network:
1. Reach **all** nodes (sufficient amount of nodes); [fault-tolerance/robustness]
2. Secure/address threat models; [security]
    - transport **correct** message (immune to falsifing msg, addressed by SGX);
    - hide the topology (anonymity);
    - malicious individuals, eclipse attack, sybil attack, ddos attack;
3. Converge as **fast** as possible (route should be optimal & msg should have no redundancy); [efficiency]
---

### Proof & Analysis of GFRR
#### Hidden Pattern/Topology
- [packet] send/out (behavior the same)
- [packat] receive/in (contact node receive info from upper-layer ring and lower-layer ring, so send fake message to prev-contact nodes in upper/lower-layer rings)
- [time] change contact node periodically, not long enough for the attacker to statistically infer the difference

#### Reach sufficient # of nodes
- hidden topology -> no specific target found -> random failure makes sense
- fault ratio analysis

#### Efficiency Analysis
- message complexity
- time complexity (communication rounds)
---

### Evaluation

Settings: 
- Traffic Control, to simulate real RTT, sleep after receiving packets if between:
    - continents: 150~200ms (random)
    - countries: 100~150ms (random)
    - cities: 50~100ms (random)
    - district: 20~50ms (random)
    - within the same district: do not sleep;

Comparison Targets:
- Kademlia (how to compare? simulate encryption-decryption time)
- Chord
- Pastry
- Tapestry

#### Time Complexity
Fixed node number: Time / # of messages/operations

#### Message Complexity
Fixed node number: # of messages sent / # of operations

#### Scalability
- Time / # of nodes (fixed # of messages/operations)
- Message / # of nodes (fixed # of operations)

#### Fault-tolerance
- Message Reachibility / Fault Ratio

#### Anonymity
- Wireshark packet watching in an interval, compare particular node & normal node

#### Maintainability
- Node Loss Recovery Time / # of existing nodes
- Node Join Boostrap Time / # of existing nodes
---

### Paper Plan

- abstract
- chap 1: introduction
- chap 2: background and motivation
- chap 3: design
- chap 4: proof & analysis
- chap 5: implementation details
- chap 6: evaluation
- chap 7: related work
- chap 8: conclusion
- ack
---

### Interface/API provided for the Adapter:
- broadcast(Message m)
- join(NodeProfile np)

### Interface/API provided for the Wire Protocol Layer
- send(Packet p, String sender_ip, String receiver_ip)