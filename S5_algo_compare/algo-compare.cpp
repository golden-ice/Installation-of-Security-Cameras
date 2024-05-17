// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <climits>

// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <mutex>

#include <atomic>

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    std::exit(EXIT_FAILURE); \
  } while (0)

// for experimental use
bool report = false;
// timeout limit for CNF-SAT-VC
int timeout = 20;
// whether CNF-SAT-VC is still calculating
std::atomic<bool> ongoing(false);
std::mutex ongoingMutex;
bool stopFlag = false;

// number of vertices
int numV = -1;
// the vector is used to store the edges
std::vector<std::pair<int, int>> edgeVec;
// the map is used to store the graph (adjacent list for each vertex)
std::map<int, std::vector<int>> givenG;

Minisat::Solver* solver;

bool isVC(Minisat::Solver* solver, std::vector<std::pair<int, int>> &E, int n, int k) {
    // reset
    // solver.reset (new Minisat::Solver());

    // initialize variables in minisat;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            solver->newVar();
        }
    }

    // At least one vertex is the ith vertex in the vertex cover
    for (int i = 0; i < k; i++) {
        Minisat::vec<Minisat::Lit> cl;
        for (int j = 0; j < n; j++) {
             cl.push(Minisat::mkLit((Minisat::Var) i + j * k));          
        }
        solver->addClause(cl);
    }

    // No one vertex can appear twice in a vertex cover
    for (int m = 0; m < n; m++) {
        for (int p = 0; p < k; p++) {
            for (int q = p + 1; q < k; q++) {
                Minisat::Lit mp = Minisat::mkLit((Minisat::Var) p + m * k);    
                Minisat::Lit mq = Minisat::mkLit((Minisat::Var) q + m * k);   
                solver->addClause(~mp, ~mq); 
            }
        }
    }


    // No more than one vertex appears in the mth position of the vertex cover.
    for (int p = 0; p < n; p++) {
        for (int q = p + 1; q < n; q++) {
            for (int m = 0; m < k; m++) {
                Minisat::Lit pm = Minisat::mkLit((Minisat::Var) m + p * k);    
                Minisat::Lit qm = Minisat::mkLit((Minisat::Var) m + q * k);   
                solver->addClause(~pm, ~qm); 
            }
        }
    }



    // Every edge is incident to at least one vertex in the vertex cover.
    for (const auto& edge : E) {
        // print
        // std::cout << "edge first: " << edge.first << ", edge second: " << edge.second << std::endl;
        Minisat::vec<Minisat::Lit> cl;
        for (int i = 0; i < k; i++) {
             cl.push(Minisat::mkLit((Minisat::Var) i + (edge.first - 1) * k));   
             cl.push(Minisat::mkLit((Minisat::Var) i + (edge.second - 1) * k));        
        }
        solver->addClause(cl);
    }

    // solve
    bool res = solver->solve();

    return res;
}

int binSearch (Minisat::Solver* solver, std::vector<std::pair<int, int>> &E, int n, int lo, int hi) {
    while (lo < hi && !stopFlag) {
        delete solver;
        solver = new Minisat::Solver();
        int mid = lo + (hi - lo + 1) / 2;
        // std::cout << mid << std::endl;
        if (isVC(solver, E, n, mid) == 0) {
            //delete solver;
            lo = mid + 1;
        } else {
            delete solver;
            solver = new Minisat::Solver();
            if (isVC(solver, E, n, mid - 1) == 1) {
                //delete solver;
                hi = mid - 1;
            } else {
                delete solver;
                solver = new Minisat::Solver();
                isVC(solver, E, n, mid);
                // std::cout << "me1" << std::endl;
                return mid;
            }
        }
    }
    delete solver;
    solver = new Minisat::Solver();
    if (stopFlag) {
        return n;
    }
    if (isVC(solver, E, n, hi) == 1) {
        // std::cout << "me2" << std::endl;
        return hi;
    } else {
        // std::cout << "hi here: "<< hi << std::endl;
        delete solver;
        solver = new Minisat::Solver();
        isVC(solver, E, n, hi + 1);
        // std::cout << t << std::endl;
        // std::cout << hi << std::endl;
        // std::cout << "me3" << std::endl;
        return hi + 1;
    }
}

void do_cal_cnf_sat_vc(int numV, std::vector<std::pair<int, int>> edgeVec) {

    solver = new Minisat::Solver();
    int minVC = binSearch(solver, edgeVec, numV, 0, numV);
    // std::cout << "minVC: " << minVC << std::endl;

    if (stopFlag) {
        stopFlag = false;
        delete solver;
        return;
    }


    // get the vertices which cover the edges
    std::vector<int> minVCVertices;
    for (int i = 0; i < numV; i++) {
        for (int j = 0; j < minVC; j++) {
            // std::cout << "test1.5" << std::endl;
            if (solver->modelValue(Minisat::mkLit((Minisat::Var) j + i * minVC)) == Minisat::l_True) {
                minVCVertices.push_back(i + 1);
            }
        }
    }

    // std::cout << "test2" << std::endl;
    std::sort(minVCVertices.begin(), minVCVertices.end());
    
    //std::lock_guard<std::mutex> lock(SATMutex);
    std::stringstream ss;
    ss << "CNF-SAT-VC: "; 
    for (std::vector<int>::iterator it = minVCVertices.begin(); it != minVCVertices.end(); ++it) {
        ss << *it;
        if (std::next(it) != minVCVertices.end()) {
        ss << ",";
        }
    }
    ss << std::endl;
    std::cout << ss.str();

    delete solver;
}

void *cal_cnf_sat_vc(void *arg) {
    // std::cout << "Working: cal_cnf_sat_vc" << std::endl;
    {
        std::lock_guard<std::mutex> lock(ongoingMutex);
        ongoing = true;
    }
    
    // std::cout << "ongoing: "<< ongoing << std::endl;
    clockid_t clock_id;
    pthread_getcpuclockid(pthread_self(), &clock_id);

    struct timespec s_time, e_time;
    clock_gettime(clock_id, &s_time);
    
    do_cal_cnf_sat_vc(numV, edgeVec);

    clock_gettime(clock_id, &e_time);
    long elapsed_microsec = (e_time.tv_sec - s_time.tv_sec) * 1000000L + (e_time.tv_nsec - s_time.tv_nsec) / 1000;

    if (report) {
        std::stringstream ssr;
        ssr << "CNF-SAT-VC Running Time: ";
        ssr << elapsed_microsec;
        ssr << " microseconds"; 
        ssr << std::endl;    
        std::cout << ssr.str();
    }
                           
    ongoing = false;
    return nullptr;
}

void do_cal_approx_vc_1(int numV, std::map<int, std::vector<int>> givenG) {
    std::vector<int> vc_1;
    // Create a new map and copy all elements from givenG
    std::map<int, std::vector<int>> copiedG1;

    for (const auto entry : givenG) {
        copiedG1[entry.first] = entry.second;
    }

    // Print copiedG1
    // for (const auto& entry : copiedG1) {
    //     std::cout << entry.first << ": ";
    //     for (int value : entry.second) {
    //         std::cout << value << " ";
    //     }
    //     std::cout << std::endl;
    // }

    int vmax = 0;
    while (!copiedG1.empty()) {
        // std::cout << "-----vertex removed in the previous iteration: " << vmax << std::endl;
        // for (const auto& entry : copiedG1) {
        //     std::cout << entry.first << ": ";
        //     for (int value : entry.second) {
        //         std::cout << value << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // Find the vertex with most edges
        auto maxEntry = copiedG1.begin();
        for (auto it = copiedG1.begin(); it != copiedG1.end(); ++it) {
            if (it->second.size() > maxEntry->second.size()) {
                maxEntry = it;
            }
        }
        vmax = maxEntry->first;
        vc_1.push_back(vmax);
        
        // Remove the incidented edges on vmax
        copiedG1.erase(maxEntry);

        for (auto it = copiedG1.begin(); it != copiedG1.end();) {
            it->second.erase(std::remove(it->second.begin(), it->second.end(), vmax), it->second.end());
            if (it->second.empty()) {
                it = copiedG1.erase(it);
            } else {
                ++it;
            }
        }

    }
    
    std::sort(vc_1.begin(), vc_1.end());

    //std::lock_guard<std::mutex> lock(APPROX1Mutex);
    std::stringstream ss;
    ss << "APPROX-VC-1: "; 
    for (std::vector<int>::iterator it = vc_1.begin(); it != vc_1.end(); ++it) {
        ss << *it;
        if (std::next(it) != vc_1.end()) {
        ss << ",";
        }
    }
    ss << std::endl;
    std::cout << ss.str();

}

void *cal_approx_vc_1(void *arg) {
    // std::cout << "Working: cal_approx_vc_1" << std::endl;
    clockid_t clock_id;
    pthread_getcpuclockid(pthread_self(), &clock_id);

    struct timespec s_time, e_time;
    clock_gettime(clock_id, &s_time);

    do_cal_approx_vc_1(numV, givenG);
    clock_gettime(clock_id, &e_time);
    long elapsed_microsec = (e_time.tv_sec - s_time.tv_sec) * 1000000L +
                      (e_time.tv_nsec - s_time.tv_nsec) / 1000;
    

    if (report) {
        // std::cout << "APPROX-VC-1 Running Time: " << elapsed_microsec << " microseconds" << std::endl;   
        std::stringstream ssr;
        ssr << "APPROX-VC-1 Running Time: ";
        ssr << elapsed_microsec;
        ssr << " microseconds"; 
        ssr << std::endl;    
        std::cout << ssr.str();
    }

    return nullptr;
}

void do_cal_approx_vc_2(int numV, std::vector<std::pair<int, int>> edgeVec) {
    std::vector<int> vc_2;
    // Create a new map and copy all elements from givenG
    std::vector<std::pair<int, int>> copiededgeVec;

    for (const auto entry : edgeVec) {
        copiededgeVec.push_back(std::make_pair(entry.first, entry.second));
    }

    int chosenVertex1 = 0;
    int chosenVertex2 = 0;
    while (!copiededgeVec.empty()) {
        // std::cout << "-----vertex removed in the previous iteration: " << chosenVertex1 << ", " << chosenVertex2 << std::endl;
        // for (const auto& edge : copiededgeVec) {
        //     std::cout << "edge first: " << edge.first << ", edge second: " << edge.second << std::endl;
        // }

        auto chosenEdge = copiededgeVec.begin();
        // push the two vertices of the chosen edge into vc_2
        chosenVertex1 = chosenEdge->first;
        chosenVertex2 = chosenEdge->second;
        vc_2.push_back(chosenVertex1);
        vc_2.push_back(chosenVertex2);

        //remove the edges incidented on the two vertices
        for (auto it = copiededgeVec.begin(); it != copiededgeVec.end();) {
            if (it->first == chosenVertex1 || it->second == chosenVertex1 || it->first == chosenVertex2 || it->second == chosenVertex2) {
                it = copiededgeVec.erase(it);
            } else {
                ++it;
            }
        }    
    }

    std::sort(vc_2.begin(), vc_2.end());
    
    //std::lock_guard<std::mutex> lock(APPROX2Mutex);
    std::stringstream ss;
    ss << "APPROX-VC-2: "; 
    for (std::vector<int>::iterator it = vc_2.begin(); it != vc_2.end(); ++it) {
        ss << *it;
        if (std::next(it) != vc_2.end()) {
        ss << ",";
        }
    }
    ss << std::endl;
    std::cout << ss.str();
}

void timeMonitor(pthread_t tid, int timeout) {
    clockid_t clock_id;
    pthread_getcpuclockid(tid, &clock_id);

    struct timespec monitor_time;  

    // std::cout << "monitoring" << std::endl;
    {
        std::lock_guard<std::mutex> lock(ongoingMutex);
        while (ongoing) {
            // std::cout << "ongoing" << std::endl;
            clock_gettime(clock_id, &monitor_time);
            // std::cout << "monitor_time.tv_sec: " << monitor_time.tv_sec << std::endl;
            if (monitor_time.tv_sec >= timeout) {
                
                
                std::cout << "CNF-SAT-VC: timeout" << std::endl; 
                solver->interrupt();
                // std::cout << "successfully interrupted!" << std::endl; 
                stopFlag = true;
                return;
            }
        }
    }
}

void *cal_approx_vc_2(void *arg) {
    // std::cout << "Working: cal_approx_vc_2" << std::endl;
    clockid_t clock_id;
    pthread_getcpuclockid(pthread_self(), &clock_id);

    struct timespec s_time, e_time;
    clock_gettime(clock_id, &s_time);

    do_cal_approx_vc_2(numV, edgeVec);
    clock_gettime(clock_id, &e_time);
    long elapsed_microsec = (e_time.tv_sec - s_time.tv_sec) * 1000000L +
                      (e_time.tv_nsec - s_time.tv_nsec) / 1000;
    
    if (report) {
        // std::cout << "APPROX-VC-2 Running Time: " << elapsed_microsec << " microseconds" << std::endl;
        std::stringstream ssr;
        ssr << "APPROX-VC-2 Running Time: ";
        ssr << elapsed_microsec;
        ssr << " microseconds"; 
        ssr << std::endl;    
        std::cout << ssr.str();
    }    

    return nullptr;  

}

void *master(void *arg) {
    // reference: the original test code given in ece650-a2.cpp

    // read from stdin until EOF
    while (!std::cin.eof()) {
        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) {
            // inputFile.close(); // Close the file
            //exit(0); // Terminate the program
            continue;
        }
        
        // create an input stream based on the line
        // we will use the input stream to parse the line
        std::istringstream input(line);

        // continue;
        // while there are characters in the input line
        while (!input.eof()) {

            char signal;

            // parse the signal: V, E, s
            input >> signal;

            // std::cout << "signal: " << signal << std::endl;
            if (signal == 'V') {
                // reset V and E
                numV = -1;
                edgeVec.clear();
                givenG.clear();

                input >> numV;
                if (numV <= 1) {
                    std::cerr << "Error: number of vertices should be greater than 1" << std::endl;
                    numV = -1;
                }
                // std::cout << "Num of Vertices: "<< numV << std::endl;
                break;
            } else if (signal == 'E') {
                std::string edges;
                
                input >> edges;
                
                edges = edges.substr(1, edges.length() - 2);
                // print
                // std::cout << edges << std::endl;   

                // if (!edgeVec.empty()) {
                //     std::cerr << "Error: Edges have already existed" << std::endl;
                //     break;
                // }

                if (numV == -1) {
                    std::cerr << "Error: vertices haven't been given" << std::endl;
                    break;
                }

                edgeVec.clear();
                givenG.clear();
                
                unsigned int lengthCounter = 0;
                std::string edgePair = "";
                char prev = '*';

                // parse the input edges
                for (char c : edges) {
                    lengthCounter += 1;
                    if ((c == ',' &&  prev == '>') || lengthCounter == edges.size()) {
                        int egdgeStart; 
                        int edgeEnd;
                        // print
                        // std::cout << edgePair << std::endl;
                        if (sscanf(edgePair.c_str(), "<%d, %d>", &egdgeStart, &edgeEnd) == 2) {
                            edgeVec.push_back({egdgeStart, edgeEnd});    
                        }
                        edgePair = "";
                    } else {
                        edgePair += c;
                    }                   
                    prev = c;
                }
                
                // put the edges to givenG
                for (const auto& edge : edgeVec) {
                    // print
                    // std::cout << "edge first: " << edge.first << ", edge second: " << edge.second << std::endl;
                    // validate the edges
                    if (edge.first < 1 || edge.first > numV || edge.second < 1 || edge.second > numV) {
                        std::cerr << "Error: Given edges contains non-exisisting vertex!" << std::endl;
                        edgeVec.clear();
                        break;
                    } else {
                        if (std::find(givenG[edge.first].begin(), givenG[edge.first].end(), edge.second) == givenG[edge.first].end()) {
                            givenG[edge.first].push_back(edge.second);
                        }   
                        if (std::find(givenG[edge.second].begin(), givenG[edge.second].end(), edge.first) == givenG[edge.second].end()) {
                            givenG[edge.second].push_back(edge.first);
                        }  
                    }  
                }

                // threads to do the 3 kinds of vc calculation
                pthread_t cnf_sat_vc, approx_vc_1, approx_vc_2;
                int ret;

                ret = pthread_create(&cnf_sat_vc, nullptr, &cal_cnf_sat_vc, nullptr);
                if (ret) {
                    handle_error("create");
                }
                
                ret = pthread_create(&approx_vc_1, nullptr, &cal_approx_vc_1, nullptr);
                if (ret) {
                    handle_error("create");
                }

                ret = pthread_create(&approx_vc_2, nullptr, &cal_approx_vc_2, nullptr);
                if (ret) {
                    handle_error("create");
                }

                timeMonitor(cnf_sat_vc, timeout);

                pthread_join(cnf_sat_vc, nullptr);
                pthread_join(approx_vc_1, nullptr);
                pthread_join(approx_vc_2, nullptr);

                break;
            } else {
                std::cerr << "Error: wrong command! =>" << signal << std::endl;
                break;
            }
            
        }

    }
    return nullptr;
}


int main(int argc, char** argv) {
    if (argc == 2) {
        if (strcmp(argv[1], "--report") == 0) {
            report = true;
        } 
    }
    pthread_t io; 
    int retcode;

    retcode = pthread_create(&io, nullptr, &master, nullptr);
    if (retcode) {
        handle_error("create");
    }

    retcode = pthread_join(io, nullptr);
    if (retcode) {
        handle_error("join");
    }

    return 0;
}
