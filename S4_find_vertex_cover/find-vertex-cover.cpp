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


// Reference1: A2 my own implementation
// Reference2：https://git.uwaterloo.ca/ece650-f23/minisat-example/-/blob/master/ece650-minisat.cpp?ref_type=heads

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
    while (lo < hi) {
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


int main(int argc, char** argv) {
    // Test code. Replaced with your code
    // reference: the original test code given in ece650-a2.cpp

    // number of vertices
    int numV = -1;
    // the map is used to store the edges
    std::vector<std::pair<int, int>> edgeVec;

    // if 1, it means at least one s command has finished successfully; 
    //     then, if we see E/V again, we should first empty numV and givenG
    // int sFinished = 0;

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
                
                // validate the edges
                for (const auto& edge : edgeVec) {
                    // print
                    // std::cout << "edge first: " << edge.first << ", edge second: " << edge.second << std::endl;
                    if (edge.first < 1 || edge.first > numV || edge.second < 1 || edge.second > numV) {
                        std::cerr << "Error: Given edges contains non-exisisting vertex!" << std::endl;
                        edgeVec.clear();
                        break;
                    } 
                }

                // do something for a4
                // std::cout << "do something for a4" << std::endl;

                Minisat::Solver* solver = new Minisat::Solver();
                int minVC = binSearch(solver, edgeVec, numV, 0, numV);
                // std::cout << "minVC: " << minVC << std::endl;

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
                for (std::vector<int>::iterator it = minVCVertices.begin(); it != minVCVertices.end(); ++it) {
                    std::cout << *it << " ";
                }
                std::cout << std::endl;

                delete solver;
                break;
            } else {
                std::cerr << "Error: wrong command! =>" << signal << std::endl;
                break;
            }
            
        }

    }
}
