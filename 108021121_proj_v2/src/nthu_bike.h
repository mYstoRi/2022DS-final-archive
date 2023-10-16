#ifndef NTHUBIKES
#define NTHUBIKES

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;

// custom classes

class bike{ // individual bikes
public:
    // stats
    int id, rental_count, type, station, arrive_time;
    double price;
    bool is_available;
};

struct bikeNode{
    bike *Bike;
    bikeNode* next;
};

class myintarray{
public:
    int size = 0, max_size;
    int *data;
    void append(int item);
    int pop(int i);
    void print();
    bool search(int item);
    bool intersect(myintarray item);
    myintarray(int input){
        max_size = input;
        data = new int[input];
    }
    myintarray(){
        max_size = 1;
        data = new int[1];
    }
};

class edge{ // unused
public:
    int start, end, cost;
};

class request{
public:
    // basic
    int user_id, start_time, arrive_time, start_station, arrive_station;
    myintarray accept_bike_type;
    // advanced
    int start_deadline;
    bool rejected;

    bool operator < (request r2);
};

class result_log{ // manage results since the output should be sorted
public:
    int user_id, bike_id, start_time, arrive_time, start_station, arrive_station, revenue;
    bool accept;
};

class queued_rq{ // (advanced)
public:
    request *Request;
    int max_profit;
};

class station{
public:
    bikeNode* root;
    int id, bike_count;
    queued_rq *rq_root;
    station(){
        root = new bikeNode;
        root->next = NULL;
        bike_count = 0;
        rq_root = NULL;
    };
    int rent(myintarray bike_type); // detect is_available to see if a bike is selected
    void arrive(int b, int arrival);
    void status(); // debug
    void sort(); // sort by id
    int rent_advanced(request *rq, int distance, bike *b); // advanced
    int has_bike_type(myintarray list);
};

struct event{
    string type;
    int occur_time;
    bike *Bike; // the true bike file is still in bikes[].
    station *Station;
    // advanced
    int distance;
    request *Request;
    /* event type list:
    "arrive" - a bike arrives their destination
    */ 
};

class event_heap{ // priority queue purpose
public:
    int size, max_size;
    event *events;
    
    event_heap(){
        size = 0;
        max_size = 10;
        events = new event[11];
    }
    bool execute(int max_rental);
    void schedule(event e);
    void what_to_do(event e);
    void realloc();
    int nextdate();
    bool is_empty();

};

// functions
    // mainly used functions
void basic(string selectedCase);
void advanced(string selectedCase);

    // tools
    int str_to_int(string input); // convert continuous numbers in string into integer
        // from the last character, add up number until a character is not a number
    myintarray bike_type_analyze(string input);
    void map_heap_insert(edge* edges, edge item, int n);
    void request_heap_insert(request* requests, request item, int n);
    request request_heap_max(request* requests, int n);
    void ShortestPath_FW(int **map, int size);
    void ac(int i); // debug, just print


// global variables
extern double depreciation;
extern int max_rental, revenue, max_station, FTS_count;
extern station *stations;
extern event_heap Events;
extern bike *bikes;
extern int **map_adj;
extern result_log *all_output, *FTS_output;
extern bool debug;

#endif