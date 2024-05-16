// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <climits>

// Reference: Dijkstra Algorithm
//            https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm

// func for calculating and outputing the shortest path between sv and ev
// return 0 means get a valid path; return -1 means no path
int shortestPathUnweighted(std::map<int, std::vector<int>>& graph, int sv, int ev) {
    std::map<int, int> predecessor;
    std::map<int, int> pathLength;
    std::vector<int> vertices;

    // If sv or ev does not connect to any of the other vertices, there is no path.
    std::map<int, std::vector<int>>::iterator it1 = graph.find(sv);
    if (it1 == graph.end()) {
        std::cerr << "Error: the start vertex is isolated!" << std::endl;
        return -1;
    }
    std::map<int, std::vector<int>>::iterator it2 = graph.find(ev);
    if (it2 == graph.end()) {
        std::cerr << "Error: the end vertex is isolated!" << std::endl;
        return -1;
    }
    
    // initialization
    for (auto& adjacency : graph) {
        predecessor[adjacency.first] = -99;
        pathLength[adjacency.first] = INT_MAX;
        vertices.push_back(adjacency.first);
    }

    pathLength[sv] = 0;

    // calculate the shortest distance from start vertex to each vertex   
    while (!vertices.empty()) {
        int minV = INT_MAX;
        int vertex = -1;
        // get the vertex with the min pathLength
        for (const auto& vertexLeft : vertices) {
            if (pathLength[vertexLeft] < minV) {
                minV = pathLength[vertexLeft];
                vertex = vertexLeft;
            }
        }
        vertices.erase(std::remove(vertices.begin(), vertices.end(), vertex), vertices.end());
        // std::cout << "I am removing: " << vertex << std::endl;
        /*
        std::cout << "Vertices: " << std::endl;
        for (int vertex : vertices) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
        */
        for (int neighbour : graph[vertex]) {
            int newLength = pathLength[vertex] + 1;
            if (newLength < pathLength[neighbour]) {
                pathLength[neighbour] = newLength;
                predecessor[neighbour] = vertex;
            }
        }
    }

    // get the shortest path from start vertex to end vertex
    if (pathLength[ev] == INT_MAX) {
        std::cerr << "Error: there is no path between start vertex and end vertex!" << std::endl;
        return -1;
    } else {
        std::vector<int> sPath;
        int pred = ev;
        while (pred != sv) {
            sPath.push_back(pred);
            // std::cout << "Vertices bakward: " << pred << std::endl; 
            pred = predecessor[pred];
        }
        sPath.push_back(pred);
        for (int i = sPath.size() - 1; i >= 0; --i) {
            std::cout << sPath[i];
            if (i > 0) {
                std::cout << "-";
            }
        }
        std::cout << std::endl;  
        return 0;         
    }
        
}

int main(int argc, char** argv) {
    // Test code. Replaced with your code
    // reference: the original test code given in ece650-a2.cpp

    // number of vertices
    int numV = -1;
    // the map is used to store the graph
    std::map<int, std::vector<int>> givenG;

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
            exit(0); // Terminate the program
        }

        if (line[0] != 's') {
            std::cout << line << std::endl;
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
                /*
                if (sFinished == 1) {
                    numV = -1;
                    givenG.clear();
                    sFinished = 0;
                }
                */
                numV = -1;
                givenG.clear();

                input >> numV;
                if (numV <= 1) {
                    std::cerr << "Error: number of vertices should be greater than 1" << std::endl;
                    numV = -1;
                }
                // std::cout << "Num of Vertices: "<< numV << std::endl;
                break;
            } else if (signal == 'E') {
                /*
                if (sFinished == 1) {
                    numV = -1;
                    givenG.clear();
                    sFinished = 0;
                }
                */
                std::string edges;
                std::vector<std::pair<int, int>> edgeVec;
                
                input >> edges;
                edges = edges.substr(1, edges.length() - 2);
                // print
                // std::cout << edges << std::endl;

                givenG.clear();

                if (numV == -1) {
                    std::cerr << "Error: vertices haven't been given" << std::endl;
                    break;
                }

                if (!givenG.empty()) {
                    std::cerr << "Error: Edges have already existed" << std::endl;
                    break;
                }
                
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
                    if (edge.first < 1 || edge.first > numV || edge.second < 1 || edge.second > numV) {
                        std::cerr << "Error: Given edges contains non-exisisting vertex!" << std::endl;
                        givenG.clear();
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

                // print
                /*
                for (const auto& edge : givenG) {
                    int key = edge.first;
                    const std::vector<int>& values = edge.second;

                    std::cout << "Key: " << key << ", Values: ";
                    for (int value : values) {
                        std::cout << value << ' ';
                    }
                    std::cout << std::endl;
                } 
                */
                break;
                
            } else if (signal == 's') {
                int start = 0;
                int end = 0;
                input >> start;
                input >> end;
                // std::cout << "num-V--: " << numV << std::endl;
                if (start < 1 || start > numV || end < 1 || end > numV) {
                    std::cerr << "Error: The start or end vertex does not exist!" << std::endl;
                    break;
                }
                // std::cout << "--Start Processing---" << std::endl;
                // int result = 99;
                // calculate the shortest path
                shortestPathUnweighted(givenG, start, end);
                // std::cout << "--End of Processing---" << std::endl;
                /*
                if (result == 0) {
                    sFinished = 1;
                }
                */
                break;
            } else {
                std::cerr << "Error: wrong command! =>" << signal << std::endl;
                break;
            }
            
        }

    }
}
