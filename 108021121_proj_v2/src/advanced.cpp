#include "./nthu_bike.h"

using namespace std;

// global variables
double depreciation;
int max_rental, revenue, max_station, FTS_count;
station *stations;
event_heap Events;
bike *bikes;
int **map_adj;
result_log *all_output, *FTS_output;
bool debug = false;

void advanced(string selectedCase)
{
    
// declarations

    // general
    int i, j, k;

    // file readers
    string file_input_s;
    int file_input_int;

    // bike infos

    int max_rental, type_count, info_size;
    int* bike_info;

    // bikes

    int id, bsize;

    // map and edge
    edge *map, insert_edge;
    int msize, mcounter, station1, station2;

    // request
    request *requests, insert_request;
    int rsize, rcounter;

    // algorithm
    int selected_index, time, max_time;
    event next_event;
    bike *selected_bike;
    result_log output;
    queued_rq *insert_queued_rq;

    // output to files
    ofstream user_result, transfer_log, station_status;
    bikeNode* output_bike;

    // debug purpose variables


// read files
    ifstream bikeF;
    ifstream mapF;
    ifstream bike_infoF;
    ifstream userF;
    bikeF.open("./testcases/" + selectedCase + "/bike.txt");
    mapF.open("./testcases/" + selectedCase + "/map.txt");
    bike_infoF.open("./testcases/" + selectedCase + "/bike_info.txt");
    userF.open("./testcases/" + selectedCase + "/user.txt");
    
// preprocess & read data

    max_time = 0;
    max_station = 0;
    // bike info
        // initialize
    type_count = 0;
    info_size = 10;
    bike_info = new int[info_size];
    bike_infoF >> depreciation;
    bike_infoF >> max_rental;
        // read
    while (true) {
        if (bike_infoF >> file_input_s);
        else break;
        bike_infoF >> file_input_int;
        if (type_count > info_size) { // reallocate memory
            int* old_bike_info = bike_info;
            bike_info = new int[info_size*2];
            for (i = 0; i < info_size; i++) bike_info[i] = old_bike_info[i];
            info_size *= 2;
            delete[] old_bike_info;
        }
        bike_info[type_count++] = file_input_int;
    }
        // resize to save memory
    int* old_bike_info = bike_info;
    bike_info = new int[type_count];
    for (i = 0; i < type_count; i++) bike_info[i] = old_bike_info[i];
    info_size = type_count;
    delete[] old_bike_info;

    // bike
    bsize = 10;
    bikes = new bike[bsize];
    while (true) {
        if (bikeF >> file_input_s);
        else break;
        if (file_input_s == "\n") break;
        bikeF >> id;
        if (bsize < id + 1) { // reallocate memory
            bike *old_bikes = bikes;
            bikes = new bike[bsize*2];
            for (i = 0; i < bsize; i++) bikes[i] = old_bikes[i];
            bsize *= 2;
            delete[] old_bikes;
        }
        bikes[id].type = str_to_int(file_input_s);
        bikes[id].id = id;
        bikeF >> file_input_s;
        bikes[id].station = str_to_int(file_input_s);
        bikeF >> bikes[id].price;
        bikeF >> bikes[id].rental_count;
        if (bikes[id].rental_count >= max_rental) bikes[id].is_available = false;
        else bikes[id].is_available = true;
    }
        // resize to save memory
    bike *old_bikes = bikes;
    bikes = new bike[id+1];
    for (i = 0; i < id+1; i++) bikes[i] = old_bikes[i];
    bsize = id+1;
    delete[] old_bikes;
    
    // map
    msize = 10;
    map = new edge[msize];
    mcounter = 0; // # of edges in map
    while (true) {
        if (mapF >> file_input_s);
        else break;
        if (file_input_s == "\n") break;
        mcounter++; 
        if (msize < mcounter+1) { // reallocate memory
            edge *old_map= map;
            map = new edge[msize*2];
            for (i = 0; i < msize; i++) map[i] = old_map[i];
            msize *= 2;
            delete[] old_map;
        }
        file_input_int = str_to_int(file_input_s);
        insert_edge.start = file_input_int;
        if (file_input_int > max_station) max_station = file_input_int;
        mapF >> file_input_s;
        file_input_int = str_to_int(file_input_s);
        insert_edge.end = file_input_int;
        if (file_input_int > max_station) max_station = file_input_int;
        mapF >> insert_edge.cost;
        /*// use min heap to store edges
        map_heap_insert(map, insert_edge, mcounter);*/
        // store regularly since we will be using adj matrix
        map[mcounter] = insert_edge;
    }
        // resize to save memory
    edge *old_map = map;
    map = new edge[mcounter+1];
    for (i = 0; i < mcounter+1; i++) map[i] = old_map[i];
    msize = mcounter+1;
    delete[] old_map;
        // create adj matrix
    max_station = map[mcounter].end+1;
    map_adj = new int*[max_station];
    for (i = 0; i < max_station; i++) {
        map_adj[i] = new int[max_station];
        map_adj[i][i] = 0;
    }
    for (i = 1; i < mcounter+1; i++){
        map_adj[map[i].start][map[i].end] = map[i].cost;
        map_adj[map[i].end][map[i].start] = map[i].cost;
    }

    // requests
    rsize = 10;
    requests = new request[rsize];
    rcounter = 0; // # of requests
    while (true) {
        
        // EOF detection (apparently .eof() does not work)
        if (userF >> file_input_s);
        else break;
        if (file_input_s == "\n") break;

        rcounter++;
        if (rsize < rcounter+1) { // reallocate memory
            request *old_requests= requests;
            requests = new request[rsize*2];
            for (i = 0; i < rsize; i++) requests[i] = old_requests[i];
            rsize *= 2;
            delete[] old_requests;
        }
        // preprocess data into buffer "insert_request"
        insert_request.user_id = str_to_int(file_input_s);
        userF >> file_input_s;
        insert_request.accept_bike_type = bike_type_analyze(file_input_s);
        userF >> insert_request.start_time;
        userF >> file_input_int;
        if(file_input_int > max_time) max_time = file_input_int;
        insert_request.arrive_time = file_input_int;
        userF >> file_input_s;
        insert_request.start_station = str_to_int(file_input_s);
        userF >> file_input_s;
        insert_request.arrive_station = str_to_int(file_input_s);
        insert_request.rejected = false;
        // use max heap to store requests first
        request_heap_insert(requests, insert_request, rcounter);
    }

    // resize to save memory
    // keep delete max from heap to reorder
    request *old_requests = requests;
    requests = new request[rcounter+1];
    for (i = rcounter; i > 0; i--) requests[i] = request_heap_max(old_requests, i);
    rsize = rcounter+1;
    delete[] old_requests;

    // record available bikes of each station
    stations = new station[max_station];
    for (i = 0; i < max_station; i++) stations[i].id = i;
    for (i = 0; i < bsize; i++) if (bikes[i].is_available) stations[bikes[i].station].arrive(i, 0);

// debug

// algorithm

    // initializations
    revenue = 0;
    all_output = new result_log[rsize];
    FTS_output = new result_log[rsize];
    FTS_count = 0;
    
    // calculate minimum distances
    ShortestPath_FW(map_adj, max_station);
    
    // record start_deadline and reject all requests with unfeasible time
    for (i = 1; i < rsize; i++) {
        requests[i].start_deadline = requests[i].arrive_time - map_adj[requests[i].start_station][requests[i].arrive_station] - 1;
        all_output[requests[i].user_id].user_id = requests[i].user_id;
        if (requests[i].start_time > requests[i].start_deadline){
            // reject request
            requests[i].rejected = true;
            if (debug)
                cout << "request >> reject request " << requests[i].user_id << " for not possible to arrive in time." << endl;
            all_output[requests[i].user_id].accept = false;
        }
    }

    // for (i = 0; i < max_station; i++) stations[i].status();

    i = 1;
    for (time = 0; time <= max_time; time++) { // tick with time
        // arrive events
        while (Events.nextdate() <= time) {
            if (!Events.execute(max_rental)) break;
        }
        // append request to queue
        // for every request here
        while(requests[i].start_time == time){
            if (requests[i].rejected) {
                i++;
                continue;
            }
                // schedule event
            next_event.type = "arrive";
            next_event.occur_time = requests[i].start_time;
            next_event.Request = &requests[i];
            next_event.distance = map_adj[requests[i].start_station][requests[i].arrive_station];
            next_event.Station = &stations[requests[i].start_station];
                // let the event heap handle actions
            Events.what_to_do(next_event);
            // next request
            i++;
        }
    }

    // pop the rest of the events
    while(Events.execute(max_rental));
    

// output
    cout << "Total revenue: " << revenue << endl;

    // user_result.txt and transfer_log.txt
    user_result.open("./result/" + selectedCase + "/user_result.txt");
    transfer_log.open("./result/" + selectedCase + "/transfer_log.txt");
    for (i = 0; i < rsize-1; i++){
        user_result << 'U' << all_output[i].user_id << ' ';
        if (all_output[i].accept) { // successful rents
            user_result << "1 ";
            user_result << all_output[i].bike_id << ' ';
            user_result << all_output[i].start_time << ' ';
            user_result << all_output[i].arrive_time << ' ';
            user_result << all_output[i].revenue << '\n';
            transfer_log << all_output[i].bike_id << " S";
            transfer_log << all_output[i].start_station << " S";
            transfer_log << all_output[i].arrive_station << ' ';
            transfer_log << all_output[i].start_time << ' ';
            transfer_log << all_output[i].arrive_time << " U";
            transfer_log << all_output[i].user_id << '\n';
        }
        else { // rejected rents
            user_result << "0 0 0 0 0\n";
        }
    }
    for (i = 0; i < FTS_count; i++){
            transfer_log << FTS_output[i].bike_id << " S";
            transfer_log << FTS_output[i].start_station << " S";
            transfer_log << FTS_output[i].arrive_station << ' ';
            transfer_log << FTS_output[i].start_time << ' ';
            transfer_log << FTS_output[i].arrive_time << " -1\n";
    }
    // station_status.txt
    station_status.open("./result/" + selectedCase + "/station_status.txt");
    for (i = 0; i < max_station; i++) {
        stations[i].sort();
        output_bike = stations[i].root;
        while (output_bike->next != NULL){
            output_bike = output_bike->next;
            station_status << 'S' << stations[i].id << ' ';
            station_status << output_bike->Bike->id << " B";
            station_status << output_bike->Bike->type << ' ';
            station_status << output_bike->Bike->price << ' ';
            station_status << output_bike->Bike->rental_count << '\n';
        }
    }

}