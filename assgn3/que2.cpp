#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;

const int MAX_BACKOFF = 6;
const int COLLISION_BACKOFF = 6;

#define num_nodes 5
#define num_rounds 40

int collisions = 0;
int transmissions = 0;
vector<int> idleTime(num_nodes + 1, 0);
vector<int> backoffcnt(num_nodes + 1, 0);

class WirelessNode
{
public:
    WirelessNode(int id) : node_id(id), backoff(0), transmitting(0), transmitting_time(1 + rand() % 5), start_time(rand() % 10) {}

    void transmit_data(vector<WirelessNode *> &v)
    {
        // if (transmitting >0) {
        //     cout << "Node " << node_id << " is already transmitting." << endl;
        //     return;
        // }

        // if (is_channel_busy())
        // {
        //     cout << "Node " << node_id << " detects channel busy." << endl;
        //     backoff = rand() % (MAX_BACKOFF + 1);
        // }
        // else

        v.push_back(this);
        // cout << "Node " << node_id << " is ready to transmit." << endl;
        // backoff = 0;
        // transmitting = true;
    }

    void update()
    {
        if (backoff > 0)
        {
            backoff--;
        }
        if (transmitting > 0)
        {
            transmitting--;
        }
    }

    bool is_ready_to_transmit()
    {
        return backoff == 0;
    }

    int node_id;
    int backoff;
    int start_time;
    int transmitting;
    int transmitting_time;
};

int is_channel_busy(vector<WirelessNode> &v)
{
    for (auto node : v)
    {

        if (node.transmitting > 0)
        {
            return node.node_id;
        }
    }
    return 0;
}

void handle_collision(vector<WirelessNode *> &nodes)
{

    for (auto node : nodes)
    {
        node->backoff = (rand() % COLLISION_BACKOFF) + 1;
        node->transmitting = 0;
        // cout << "Collision detected for Node " << node->node_id << ". Backing off. with backoff= " << node->backoff << endl;
        //  collisions++;
        printf("\033[1;31mCollision detected for Node %d. Backing off. with backoff= %d\033[0m\n", node->node_id, node->backoff);
        idleTime[node->node_id]++;
        backoffcnt[node->node_id]++;
    }
}

int main()
{

    srand(time(nullptr)); // Initialize the random number generator

    vector<WirelessNode> nodes;
    for (int i = 0; i < num_nodes; i++)
    {
        nodes.push_back(WirelessNode(i + 1));
        cout << "\033[1;37mNode: " << nodes[i].node_id << ", start time: " << nodes[i].start_time << ", transmitting time: " << nodes[i].transmitting_time << "\033[0m" << endl;
    }

    for (int round = 0; round < num_rounds; round++)
    {
        cout << "\033[1;37mRound " << round + 1 << ":"
             << "\033[0m" << endl;

        vector<WirelessNode *> can_transmit;

        int busy_node = is_channel_busy(nodes);
        for (WirelessNode &node : nodes)
        {
            if (node.start_time <= round)
            {
                if (node.is_ready_to_transmit())
                {

                    if (busy_node > 0)
                    {

                        printf("\033[1;31mBUSY CHANNEL!! \033[0m");
                        if (node.node_id != busy_node)
                        {
                            // cout << "Node " << node.node_id << " detects channel being used by " << busy_node << endl;
                            printf("\033[1;31mNode %d detects channel being used by %d \033[0m\n", node.node_id, busy_node);
                            node.backoff = rand() % (MAX_BACKOFF + 1);
                            backoffcnt[node.node_id]++;
                            idleTime[node.node_id]++;
                        }
                        else
                        {
                            // cout << "Node " << busy_node << " is already transmitting" << endl;
                            printf("\033[1;32mNode %d is already transmitting \033[0m\n", busy_node);
                            // cout << node.backoff << " " << node.transmitting << " is the backoff and time left" << endl;
                            transmissions++;
                        }
                    }
                    else
                    {
                        node.transmit_data(can_transmit);
                    }
                }
                else
                {
                    // cout << "Node " << node.node_id << " is waiting (backoff = " << node.backoff << ")." << endl;
                    printf("\033[1;33mNode %d is waiting (backoff = %d). \033[0m\n", node.node_id, node.backoff);
                    idleTime[node.node_id]++;
                }
            }
            else
            {
                // cout<<"Node "<<node.node_id<<" has not came yet "<<endl;
                printf("\033[1;34mNode %d has not come yet. \033[0m\n", node.node_id);
                idleTime[node.node_id]++;
            }
            // if (node.is_ready_to_transmit())
            // {

            //     if (busy_node > 0)
            //     {
            //         cout << "BUSY CHANNEL!! ";
            //         if (node.node_id != busy_node)
            //         {
            //             cout << "Node " << node.node_id << " detects channel being used by " << busy_node << endl;
            //             node.backoff = rand() % (MAX_BACKOFF + 1);
            //             backoffcnt[node.node_id]++;
            //             idleTime[node.node_id]++;
            //         }
            //         else
            //         {
            //             cout << "Node " << busy_node << " is already transmitting" << endl;
            //             cout << node.backoff << " " << node.transmitting << " is the backoff and time left" << endl;
            //             transmissions++;
            //         }
            //     }
            //     else
            //     {
            //         node.transmit_data(can_transmit);
            //     }
            // }
            // else
            // {
            //     cout << "Node " << node.node_id << " is waiting (backoff = " << node.backoff << ")." << endl;
            //     idleTime[node.node_id]++;
            // }
        }
        if (can_transmit.size() == 1)
        {
            transmissions++;
            // cout << "Node " << can_transmit[0]->node_id << " starts transmitting" << endl;
            printf("\033[1;32mNode %d starts transmitting \033[0m\n", can_transmit[0]->node_id);
            can_transmit[0]->transmitting = can_transmit[0]->transmitting_time;
            can_transmit[0]->backoff = 0;
        }
        else
        {
            if (can_transmit.size() > 1)
                collisions++;

            // for(auto node: can_transmit ){

            //     idleTime[node->node_id+1]++;
            // }

            handle_collision(can_transmit);
        }

        for (WirelessNode &node : nodes)
        {
            node.update();
        }

        this_thread::sleep_for(chrono::seconds(0)); // Simulate a time delay between rounds
        cout << "\033[1;37m______________________________________________________________________________________"
             << "\033[0m" << endl<<endl;
    }

    cout << "\033[1;37mStats:\033[0m" << endl;
    cout << "\033[1;37mTransmission time (amount of time slots effectively used): " << transmissions << "\033[0m" << endl;
    cout << "\033[1;37mCollision time (amount of time slots where collision occured): " << collisions << "\033[0m" << endl;
    cout << "\033[1;37m______________________________________________________________________________________"
         << "\033[0m" << endl<<endl;

    for (int i = 0; i < num_nodes; i++)
    {
        cout << "\033[1;37mNode " << i + 1 << "'s idle time : " << idleTime[i + 1] << "\033[0m" << endl;
    }
    cout << "\033[1;37m______________________________________________________________________________________"
         << "\033[0m" << endl<<endl;

    for (int i = 0; i < num_nodes; i++)
    {
        cout << "\033[1;37mNode " << i + 1 << "'s no. of times backed off : " << backoffcnt[i + 1] << "\033[0m" << endl;
    }
    return 0;
}