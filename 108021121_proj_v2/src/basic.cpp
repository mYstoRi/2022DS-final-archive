#include "./nthu_bike.h"
#include "./functions.h"

using namespace std;

void basic(string selectedCase){

// declarations

    // general
    int i, j, k;

    // file readers
    string file_input_s;
    int file_input_int;

    // bike infos
    int type_count, info_size;
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
    int revenue_increment, selected_index;
    event next_event;
    bike *selected_bike;
    result_log output;

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

    // bike (temporary)
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
        insert_edge.start = str_to_int(file_input_s);
        mapF >> file_input_s;
        insert_edge.end = str_to_int(file_input_s);
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
        userF >> insert_request.arrive_time;
        userF >> file_input_s;
        insert_request.start_station = str_to_int(file_input_s);
        userF >> file_input_s;
        insert_request.arrive_station = str_to_int(file_input_s);
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

// algorithm

    // calculate minimum distances
    ShortestPath_FW(map_adj, max_station);
    
    // initializations
    revenue = 0;
    all_output = new result_log[rsize];

    for (i = 1; i < rsize; i++) { // request in order, i = current request index

            // uses an "event handler" style DS to handle arrives.
            // The system help reduces "if checks" for every bike everytime time changes
            // kind of like interrupts instead of pollings

        // go through all events that happened until the current request
        while (Events.nextdate() <= requests[i].start_time) { 
            // pop events until current time
            if (!Events.execute(max_rental)) break;
        }
        // process request
            // check (1)if the station has any bikes, (2)if it is possible to arrive in time
        if (map_adj[requests[i].start_station][requests[i].arrive_station] + requests[i].start_time < requests[i].arrive_time){
            selected_index = stations[requests[i].start_station].rent(requests[i].accept_bike_type);
            selected_bike = &bikes[selected_index];
            if (selected_index >= 0){
                // yes, select the bike with highest price > lowest id (included in selected_bike)
                    // add it to the event
                next_event.occur_time = requests[i].start_time + map_adj[requests[i].start_station][requests[i].arrive_station];
                next_event.type = "arrive";
                next_event.Bike = selected_bike;
                next_event.Station = &stations[requests[i].arrive_station];
                Events.schedule(next_event);
                    // then, add the cost to the revenue
                revenue_increment = selected_bike->price * map_adj[requests[i].start_station][requests[i].arrive_station];
                revenue += revenue_increment;
                    // add rental count
                selected_bike->rental_count++;
                selected_bike->price -= depreciation;
                    // set is_available to false
                selected_bike->is_available = false;
                    // output
                if (debug) {
                    cout << "[time: " << requests[i].start_time << "] ";
                    cout << "request >> accept request " << requests[i].user_id << " renting bike " << selected_index << endl;
                }
                output.user_id = requests[i].user_id;
                output.accept = true;
                output.bike_id = selected_bike->id;
                output.start_time = requests[i].start_time;
                output.arrive_time = map_adj[requests[i].start_station][requests[i].arrive_station] + requests[i].start_time;
                output.start_station = requests[i].start_station;
                output.arrive_station = requests[i].arrive_station;
                output.revenue = revenue_increment;
            }
            else {
                // reject for there is no bikes available
                if (debug) {
                    cout << "[time: " << requests[i].start_time << "] ";
                    cout << "request >> reject request " << requests[i].user_id << " for no available bikes." << endl;
                }
                output.user_id = requests[i].user_id;
                output.accept = false;
            }
        }
        else{
                // reject for not possible to arrive in time
                if (debug) {
                    cout << "[time: " << requests[i].start_time << "] ";
                    cout << "request >> reject request " << requests[i].user_id << " for not possible to arrive in time." << endl;
                }
                output.user_id = requests[i].user_id;
                output.accept = false;
        }
        all_output[requests[i].user_id] = output;
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

// file query debug (file checker for user_result.txt)
/*
    if (debug){
        ifstream debugFile, sampleFile;
        int file_input_int1, file_input_int2, min1 = 2000, target = -1, trash;
        string file_input_s1, file_input_s2;
        debugFile.open("./result/" + selectedCase + "/user_result.txt");
        sampleFile.open("./ans_v2/sorted_ans/" + selectedCase + "/user_result.txt"); // need config (directory based)

        while (true) {
            if (debugFile >> file_input_s && sampleFile >> file_input_s);
            else break;
            debugFile >> trash;
            sampleFile >> trash;
            debugFile >> file_input_int1;
            sampleFile >> file_input_int2;
            debugFile >> file_input_int;
            sampleFile >> trash;
            if (trash > file_input_int) file_input_int = trash;
            if (file_input_int1 != file_input_int2){
                cout << file_input_s << ": " << file_input_int1 << "=/=" << file_input_int2 << endl;
                if (min1 > file_input_int) {
                    min1 = file_input_int;
                    target = str_to_int(file_input_s);
                }   
            }
            debugFile >> trash;
            debugFile >> trash;
            sampleFile >> trash;
            sampleFile >> trash;
        }   
        if (target >= 0) cout << "check out #" << target << endl;
        else cout << "everything seems fine!\n";
    }
*/

}