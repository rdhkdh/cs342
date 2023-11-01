// assignment 3, ques 1, group- CS18
// 210101088, 210101045, 210101075

#include<bits/stdc++.h>
using namespace std;

class Router{
    public:
    int router_id;
    map<Router*,int> neighbours; //ptr to node, edge length
    map<int, pair<int,int>> forwarding_table; //destn id, pair(cost,edge)

    Router(int id, int N)  //constructor
    { 
        router_id = id; 
        for(int i=0;i<N;i++)
        {
            forwarding_table[i] = make_pair(INT_MAX,router_id);
        }
        forwarding_table[router_id] = make_pair(0,router_id); //to itself
        Router* self_ptr = this;
    }

    void add_neighbour(Router* node, int edge_length) 
    {
        neighbours[node] = edge_length;
        node->neighbours[this] = edge_length;

        forwarding_table[node->router_id] = make_pair(edge_length,node->router_id); //cost,edge
        node->forwarding_table[router_id] = make_pair(edge_length,router_id);
    }

    // applies Dijkstra's Algorithm
    void update_routing_table(int N, set<Router*> all_nodes) //total no of nodes
    {
        set<Router*> included;
        Router* min_cost_node = this;
        int count=0;

        while(count<N)
        {
            included.insert(min_cost_node); 
            for(auto b: min_cost_node->neighbours)
            {
                // D(b)= min( D(b), D(a)+C_ab ) for all b: neighbours(a)
                int Db = forwarding_table[b.first->router_id].first; //cost
                int Da = forwarding_table[min_cost_node->router_id].first; //cost
                int C_ab = b.second; //edge length

                if(Da+C_ab < Db)
                {
                    //update cost
                    forwarding_table[b.first->router_id].first = Da+C_ab;
                    //update edge to take
                    if(min_cost_node!=this)
                    {
                        forwarding_table[b.first->router_id].second = forwarding_table[min_cost_node->router_id].second;
                    }
                    else
                    {
                        forwarding_table[b.first->router_id].second = b.first->router_id;
                    }
                }
                //else: no changes to do 
            }

            //find min cost node for next iteration
            int min_cost = INT_MAX;
            for(auto v: all_nodes)
            {
                if(included.find(v)==included.end() && forwarding_table[v->router_id].first < min_cost)
                {
                    min_cost_node = v;
                    min_cost = forwarding_table[v->router_id].first;
                }
            }

            count++;
        }

    }

    void print_routing_table()
    {
        cout<<"Routing table for node ID "<<router_id<<":"<<endl;
        for(auto u: forwarding_table)
        {
            cout<<"Node "<<router_id<<" to "<<u.first<<": Cost= "<<u.second.first<<", Take edge: ("<<router_id<<","<<u.second.second<<")"<<endl;
        }
        cout<<endl;
    }
};



int main()
{
    Router r0(0,6),r1(1,6),r2(2,6),r3(3,6),r4(4,6),r5(5,6); //N=6
    set<Router*> all_nodes = {&r0,&r1,&r2,&r3,&r4,&r5};
    
    r0.add_neighbour(&r1,2); 
    r0.add_neighbour(&r2,1); 
    r0.add_neighbour(&r3,5); 
    r1.add_neighbour(&r2,2); 
    r1.add_neighbour(&r3,3);
    r2.add_neighbour(&r3,3);
    r2.add_neighbour(&r4,1);
    r3.add_neighbour(&r4,1);
    r3.add_neighbour(&r5,5);
    r4.add_neighbour(&r5,2);

    // cout<<"Before applying OSPF protocol:\n";
    // r0.print_routing_table();
    // r1.print_routing_table();
    // r2.print_routing_table();
    // r3.print_routing_table();
    // r4.print_routing_table();
    // r5.print_routing_table();

    r0.update_routing_table(6,all_nodes);
    r1.update_routing_table(6,all_nodes);
    r2.update_routing_table(6,all_nodes);
    r3.update_routing_table(6,all_nodes);
    r4.update_routing_table(6,all_nodes);
    r5.update_routing_table(6,all_nodes);

    cout<<"After applying OSPF protocol:\n\n";
    r0.print_routing_table();
    r1.print_routing_table();
    r2.print_routing_table();
    r3.print_routing_table();
    r4.print_routing_table();
    r5.print_routing_table();

    return 0;
}