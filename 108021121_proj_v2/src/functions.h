#ifndef FUNCTIONS
#define FUNCTIONS

#include <iostream>
#include <fstream>
#include <string.h>
#include "nthu_bike.h"

using namespace std;

// tool function definitions
int str_to_int(string input){

    int sum = 0, exp = 1, i, n = input.length();

    i = n-1;
    while (i>=0) {
        switch (input[i])
        {
        case '0':
            break;
        case '1':
            sum += exp;
            break;
        case '2':
            sum += 2 * exp;
            break;
        case '3':
            sum += 3 * exp;
            break;
        case '4':
            sum += 4 * exp;
            break;
        case '5':
            sum += 5 * exp;
            break;
        case '6':
            sum += 6 * exp;
            break;
        case '7':
            sum += 7 * exp;
            break;
        case '8':
            sum += 8 * exp;
            break;
        case '9':
            sum += 9 * exp;
            break;
        case '-':
            sum = -sum;
            break;
        default:
            break;
        }
        i--;
        exp *= 10;
    }
    return sum;
}

myintarray bike_type_analyze(string input){
    string *substrings, tmp;
    int n=1, i, j;
    // determine how many types are accepted
    for (i = 0; i < input.length()+1; i++) if (input[i] == ',') n++;
    // allocate memory
    substrings = new string[n];
    myintarray results(n);
    // parse substrings
    j = 0;
    for (i = 0; i < input.length()+1; i++){
        if (input[i] != ',' && input[i] != '\0') tmp = tmp + input[i];
        else {
            substrings[j] = tmp;
            tmp = "";
            j++;
        }
    }
    // process substrings into int array
    for (i = 0; i < j; i++) results.append(str_to_int(substrings[i]));
    return results;
}

void map_heap_insert(edge* edges, edge item, int n){
    int current = n, parent;
    bool done = false;
    while (current > 1 && !done){
        parent = current/2;
        if (edges[parent].cost > item.cost){
            edges[current] = edges[parent];
            current /= 2;
        }
        else done = true;
    }
    edges[current] = item;
}

void request_heap_insert(request* requests, request item, int n){
    int current = n, parent;
    bool done = false;
    while (current > 1 && !done){
        parent = current/2;
        if (requests[parent] < item){
            requests[current] = requests[parent];
            current /= 2;
        }
        else done = true;
    }
    requests[current] = item;
}

request request_heap_max(request* requests, int n){

    if (n == 1) return requests[1];

    request item = requests[n], max = requests[1];
    int i = 1, j = i*2;
    bool done = false;
    requests[1] = requests[n];
    while (j < n && !done){
        if (j < n-1 && requests[j] < requests[j+1]) j++;
        if (item < requests[j]) {
            requests[i] = requests[j];
            i = j;
            j *= 2;
        }
        else done = true;
    }
    requests[i] = item;
    return max;
}

void ShortestPath_FW(int **map, int size){
    int start, mid, end;
    for (mid = 0; mid < size; mid++){
        for (start = 0; start < size; start++){
            if (start == mid) continue;
            for (end = 0; end < size; end++){
                if (start == end || mid == end) continue;
                if (map[start][end] > map[start][mid]+map[mid][end]){
                    map[start][end] = map[start][mid]+map[mid][end];
                }
            }
        }
    }
}

void ac(int i){ // alive check? alive check! hehe *laughs with insanity*
    cout << "alive check " << i << endl;
}

// class functions

    // request comparison
bool request::operator < (request r2){
    if (start_time < r2.start_time) return true;
    else if (start_time == r2.start_time && user_id < r2.user_id) return true;
    return false;
}

    // station
int station::rent(myintarray bike_type){
    bikeNode *current = root, *b4current, *selected, *b4selected;
    double max;
    int output = -1;
    while (current->next != NULL){ // search through all bikes at station
        b4current = current;
        current = current->next;
        if (bike_type.search(current->Bike->type) && current->Bike->is_available){ // match type
            if (max < current->Bike->price) { // found a more expensive bike
                max = current->Bike->price;
                output = current->Bike->id;
                selected = current;
                b4selected = b4current;
            }
            else if (max == current->Bike->price && output > current->Bike->id) {
                // tiebreaker
                max = current->Bike->price;
                output = current->Bike->id;
                selected = current;
                b4selected = b4current;
            }
        }
    }
    // delete selected bike
    if (output >= 0){
        b4selected->next = selected->next;
        bike_count--;
    }
    return output;
}

void station::arrive(int b, int arrival){
    bikeNode *insert = new bikeNode;
    insert->Bike = &bikes[b];
    insert->Bike->arrive_time = arrival;
    if (root->next != NULL) insert->next = root->next;
    else insert->next = NULL;
    root->next = insert;
    bike_count++;
}

void station::status(){
    bikeNode *b = root;
    cout << "station " << id << " status:\n  Available bikes:\n";
    while (b->next != NULL) {
        b = b->next;
        cout << "    bike: " << b->Bike->id << " type: " << b->Bike->type;
        cout << ", price: " << b->Bike->price << endl;
    }
}

void station::sort(){ // insertion (other sorts don't improve for linked list)
    bikeNode *current, *insert_node, *find;
    insert_node = root;
    while (insert_node->next != NULL){
        current = insert_node;
        insert_node = insert_node->next;
        find = root;
        while (find->next->Bike->id < insert_node->Bike->id) find = find->next;
        current->next = insert_node->next;
        insert_node->next = find->next;
        find->next = insert_node;
        if (find != current) insert_node = current;
    }
}

int station::rent_advanced(request *rq, int distance, bike *b){ // advanced
    int revenue_increment;
    event next_event;
    next_event.occur_time = rq->start_time + distance;
    next_event.type = "arrive";
    next_event.Bike = b;
    next_event.Station = &stations[rq->arrive_station];
        // cout section
    Events.schedule(next_event);
    // then, add the cost to the revenue
    revenue_increment = b->price * distance;
    revenue += revenue_increment;
        // add rental count
    b->rental_count++;        
    b->price -= depreciation;
        // set is_available to false
    b->is_available = false;
    return revenue_increment;
}

int station::has_bike_type(myintarray list){
    int count = 0;
    bikeNode *current = root;
    while (current->next != NULL) {
        current = current->next;
        if (list.search(current->Bike->type)) count++;
    }
    return count;
}

    // myintarray
void myintarray::append(int item){
    int i;
    // reallocate if necessary
    if (size > max_size) {
        int* old_data = data;
        data = new int[max_size*2];
        for (i = 0; i < size; i++) data[i] = old_data[i];
        max_size *= 2;
        delete[] old_data;
    }
    data[size++] = item;
}

void myintarray::print(){
    int i;
    for (i = 0; i < size; i++) {
        cout << data[i];
        if (i < size-1) cout << ", ";
    }
}

int myintarray::pop(int i){
    if (size == 0) throw("myintarray: attempt to pop with nothing in it.");
    int result = data[i];
    data[i] = data[size--];
    return result;
}

bool myintarray::search(int item){
    int i;
    for (i = 0; i < size; i++) {
        if (item == data[i]) return true;
    }
    return false;
}

bool myintarray::intersect(myintarray item){
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < item.size; j++) {
            if (item.data[j] == data[i]) return true;
        }
    }
    return false;
}

    // event heap
bool event_heap::execute(int max_rental){
        // returns true if executed, false if there is nothing left in the queue
    if (is_empty()) return false;

    int i;
    event e = events[1], heapE;
    bool done;

    // maintain min heap
    heapE = events[size];
    i = 2;
    done = false;
    while (i < size && !done){
        if (i < size-1 && events[i].occur_time > events[i+1].occur_time) i++;
        if (events[i].occur_time < heapE.occur_time) {
            events[i/2] = events[i];
            i *= 2;
        }
        else done = true;
    }
    events[i/2] = heapE;
    size--;

    // arrive events
    if (e.type == "arrive") {
        if (e.Bike->rental_count < max_rental) {
            // cout section
            if (debug) {
                cout << "[time: " << e.occur_time << "] ";
                cout << "event >> bike " << e.Bike->id << " arrive at station " << e.Station->id << endl;
            }
            // add to station
            e.Bike->is_available = true;
            e.Station->arrive(e.Bike->id, e.occur_time);
            e.Bike->station = e.Station->id;
        }
        else { // retire
            if (debug)
                cout << "Bike " << e.Bike->id << " retired.\n";
            e.Station->arrive(e.Bike->id, e.occur_time);
            e.Bike->station = e.Station->id;
            e.Bike->is_available = false;
        }
    }
    return true;
}

void event_heap::schedule(event e){

        // use min heap to manage

    int i = size+1;

    // check if has enough space
    if (i+1 > max_size) realloc();

    // insert
    while (i/2 > 0 && events[i/2].occur_time > e.occur_time) {
        events[i] = events[i/2];
        i /= 2;
    }
    events[i] = e;
    size++;
}

void event_heap::realloc(){
    event *old_events = events;
    events = new event[max_size*2];
    for (int i = 1; i <= size; i++) events[i] = old_events[i];
    max_size *= 2;
    delete[] old_events;
}

int event_heap::nextdate(){
    if (is_empty()) return -1;
    return events[1].occur_time;
}

bool event_heap::is_empty(){
    return size == 0;
}

void event_heap::what_to_do(event e){
    double max;
    int index = -1, i, revenue_increment, selected_index;
    bool check = false;
    bikeNode *selected_bike, *b4selected_bike, *current_bike, *b4current_bike;

    /* logic:
    if found -> rent
    else 
        if arriving -> reserve
        else
            if FTS possible -> FTS
            else reject
    */

    // if there are available bikes, rent the highest immediately
        // choose the highest bike with matching type
    current_bike = e.Station->root;
    b4current_bike = e.Station->root;
    max = 0;
    while (current_bike->next != NULL) {
        b4current_bike = current_bike;
        current_bike = current_bike->next;
        if (e.Request->accept_bike_type.search(current_bike->Bike->type) &&
            current_bike->Bike->price > max &&
            current_bike->Bike->is_available) {
            max = current_bike->Bike->price;
            index = current_bike->Bike->id;
            selected_bike = current_bike;
            b4selected_bike = b4current_bike;
        }
    }
    if (index >= 0){ // found bike
        // rent
        b4selected_bike->next = selected_bike->next;
        revenue_increment = e.Station->rent_advanced(e.Request, e.distance, &bikes[selected_bike->Bike->id]);
            // output
        if (debug) {
            cout << "[time: " << e.Request->start_time << "] ";
            cout << "request >> request " << e.Request->user_id << " rent bike ";
            cout << selected_bike->Bike->id << ", arrive at " << e.Request->start_time + e.distance;
            cout << ".\n";
        }
        all_output[e.Request->user_id].accept = true;
        all_output[e.Request->user_id].bike_id = index;
        all_output[e.Request->user_id].start_time = e.Request->start_time;
        all_output[e.Request->user_id].arrive_time = e.Request->start_time + e.distance;
        all_output[e.Request->user_id].revenue = revenue_increment;
        all_output[e.Request->user_id].start_station = e.Request->start_station;
        all_output[e.Request->user_id].arrive_station = e.Request->arrive_station;
    }
    else { // else if it has time to wait, check if there are bikes arriving
        for (i = 1; i <= Events.size; i++) {
            if (events[i].type == "arrive" && 
                events[i].occur_time < e.Request->start_deadline &&
                events[i].Station->id == e.Request->start_station &&
                events[i].Bike->rental_count < max_rental - 1 &&
                e.Request->accept_bike_type.search(events[i].Bike->type)) check = true;
            if (check) break;
        }
        if (check) { // "reserves" the bike
                // cout
            if (debug) {
                cout << "[time: " << events[i].occur_time << "] ";
                cout << "request >> request " << e.Request->user_id << " reserves bike ";
                cout << events[i].Bike->id << ", now arriving at ";
                cout << events[i].occur_time + e.distance << ".\n";
            }
            // extend the event, change it's destination
            events[i].occur_time += e.distance;
            events[i].Station = &stations[e.Request->arrive_station];
            // add revenue
            revenue_increment = e.distance*events[i].Bike->price;
            revenue += revenue_increment;
                // output
            all_output[e.Request->user_id].accept = true;
            all_output[e.Request->user_id].start_time = events[i].occur_time;
            all_output[e.Request->user_id].arrive_time = events[i].occur_time + e.distance;
            all_output[e.Request->user_id].bike_id = events[i].Bike->id;
            all_output[e.Request->user_id].revenue = revenue_increment;
            all_output[e.Request->user_id].start_station = e.Request->start_station;
            all_output[e.Request->user_id].arrive_station = e.Request->arrive_station;
            // add rental again
            events[i].Bike->price -= depreciation;
            events[i].Bike->rental_count++;
            // heapify the heap from there
            int j = 2*i;
            bool done = false;
            event tmp = events[i];
            while (j <= size && !done){
                if (j < size && events[j].occur_time > events[j+1].occur_time) j++;
                if (tmp.occur_time < events[j].occur_time) done = true;
                else {
                    events[j/2] = events[j];
                    j *= 2;
                }
            }
            events[j/2] = tmp;
        }
        else {  // else if FTS can arrive in time, call for FTS
            check = false;
            for (i = 0; i < max_station; i++) {
                if (i == e.Request->start_station) continue;
                if (map_adj[e.Request->start_station][i] < e.Request->start_deadline - e.Request->start_time) {
                    selected_index = stations[i].rent(e.Request->accept_bike_type);
                    if (selected_index >= 0) {
                        check = true;
                        // cout section
                        if (debug){
                            cout << "[time: " << e.Request->start_time << "] ";
                            cout << "FTS >> transfer bike " << selected_index << " to station ";
                            cout << e.Station->id << " and arrive at ";
                            cout << e.Request->start_time + map_adj[e.Request->start_station][i] << ".\n";
                            cout << "[time: " << e.Request->start_time + map_adj[e.Request->start_station][i] << "] ";
                            cout << "request >> request " << e.Request->user_id << " renting bike ";
                            cout << selected_index << ". Arrive at ";
                            cout << e.Request->start_time + map_adj[e.Request->start_station][i] + e.distance << ".\n";
                        }
                        // set unavailable
                        bikes[selected_index].is_available = false;
                        // use FTS from station i to the start station
                        e.occur_time = e.Request->start_time + map_adj[e.Request->start_station][i] + e.distance;
                        // schedule arrive
                        e.Bike = &bikes[selected_index];
                        e.Station = &stations[e.Request->arrive_station];
                        schedule(e);
                        // calculate revenue
                        revenue_increment = bikes[selected_index].price*e.distance;
                        revenue += revenue_increment;
                        // record FTS + user
                        all_output[e.Request->user_id].accept = true;
                        all_output[e.Request->user_id].start_time = e.Request->start_time + map_adj[e.Request->start_station][i];
                        all_output[e.Request->user_id].arrive_time = 
                                        e.Request->start_time + map_adj[e.Request->start_station][i] + e.distance;
                        all_output[e.Request->user_id].bike_id = selected_index;
                        all_output[e.Request->user_id].revenue = revenue_increment;
                        all_output[e.Request->user_id].start_station = e.Request->start_station;
                        all_output[e.Request->user_id].arrive_station = e.Request->arrive_station;
                        FTS_output[FTS_count].accept = true;
                        FTS_output[FTS_count].start_time = e.Request->start_time;
                        FTS_output[FTS_count].arrive_time = e.Request->start_time + map_adj[e.Request->start_station][i];
                        FTS_output[FTS_count].bike_id = e.Bike->id;
                        FTS_output[FTS_count].revenue = 0;
                        FTS_output[FTS_count].start_station = i;
                        FTS_output[FTS_count].arrive_station = e.Request->start_station;
                        FTS_count++;
                        // add rental
                        bikes[selected_index].price -= depreciation;
                        bikes[selected_index].rental_count++;
                        break;
                    }
                }
            }
            if (!check) {
                // else reject
                e.Request->rejected = true;
                // output
                if (debug)
                    cout << "request >> request " << e.Request->user_id << " rejected.\n";
                all_output[e.Request->user_id].accept = false;
            }
        }
    }
}

#endif
