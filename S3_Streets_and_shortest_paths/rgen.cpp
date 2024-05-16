
#include <fstream>
#include <iostream>
#include <getopt.h>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <unistd.h>

int randGen(void) {
    std::ifstream urandom("/dev/urandom");
    if (urandom.fail()) {
        std::cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }

    // read a random 8-bit value.
    char ch = 'a';
    urandom.read(&ch, 1);
    // cast to integer to see the numeric value of the character
    int randnum = (int) ch;
    // printTest
    // std::cout << "Random character: " << randnum << "\n";

    // close random stream
    urandom.close();

    return randnum;    
}

int setStreetNum(int min, int max) {
    int streetNum = randGen();
    while (streetNum < min || streetNum > max) {
        streetNum = randGen();
    }
    return streetNum;
}

std::vector<int> setLineSegNum(int streetNum, int min, int max) {
    std::vector<int> lineSegNum;
    for (int i = 0; i < streetNum; ++i) {
        int lineSegNumEle = randGen();
        while (lineSegNumEle < min || lineSegNumEle > max) {
            lineSegNumEle = randGen();
        }
        lineSegNum.push_back(lineSegNumEle);
    }
    return lineSegNum;
}

int setWaitTime(int min, int max) {
    int waitTime = randGen();
    while (waitTime < min || waitTime > max) {
        waitTime = randGen();
    }
    return waitTime;
}


std::vector<std::string> setStreetNames(int streetNum) {
    std::vector<std::string> streetNames;
    for (int i = 0; i < streetNum; ++i) {
        std::string streetName = "s" + std::to_string(i);;
        streetNames.push_back(streetName);
    }
    return streetNames;   
}

std::vector<std::vector<std::pair<int, int>>> setCoords(std::vector<int> lineSegNum, int limit) {
    std::vector<std::vector<std::pair<int, int>>> coords;
    for (int i = 0; i < (int) lineSegNum.size(); i++) {
        std::vector<std::pair<int, int>> inner;
        for (int j = 0; j < lineSegNum[i] + 1; j++) {
            int first = randGen();
            int second = randGen();
            while (first < -limit || first > limit) {
                first = randGen();
            }
            while (second < -limit || second > limit) {
                second = randGen();
            }
            std::pair<int, int> coord(first, second);
            inner.push_back(coord);
        }
        coords.push_back(inner);
    }
    return coords;
}

/*
// if intersectionCheck = 0, it means no intersection between the two line segments;
// if intersectionCheck = 1, it means there is intersection between the two line segments;
int intersectionCheck(std::vector<std::pair<int, int>> lineSeg1, std::vector<std::pair<int, int>> lineSeg2) {
    return 0;
}
*/

// if return 0, it means no overlapping between any two line segments;
// if return 1, it means there is overlapping between some two line segments;
int overlapCheck(std::vector<std::pair<int, int>> lineSeg1, std::vector<std::pair<int, int>> lineSeg2) {
    // lineSeg1(P1, P2), lineSeg1(P3, P4)
    int x1 = lineSeg1[0].first;
    int y1 = lineSeg1[0].second;
    int x2 = lineSeg1[1].first;
    int y2 = lineSeg1[1].second;
    int x3 = lineSeg2[0].first;
    int y3 = lineSeg2[0].second;
    int x4 = lineSeg2[1].first;
    int y4 = lineSeg2[1].second;

    //P1--P2, P3--P4 should not have a same slope
    int denomi = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    //P1--P2, P1--P3(virtual) should not have a same slope
    int denomiVirtual = (y2 - y1) * (x3 - x1) - (y3 - y1) * (x2 - x1);

    // if denomi=0, the two line segments are overlapping or parallel
    if (denomi == 0) {
        // if denomiVirtual=0, the two line segments are on the same line
        if (denomiVirtual == 0) {
            // different cases of overlapping
            if ((x3 > x1 && x3 < x2) || (x4 > x1 && x4 < x2) || (x1 > x3 && x1 < x4) || (x2 > x3 && x2 < x4)) {
                return 1;
            } else if ((x3 < x1 && x3 > x2) || (x4 < x1 && x4 > x2) || (x1 > x4 && x1 < x3) || (x2 > x4 && x2 < x3)) {
                return 1;
            } else if (x1 == x3) {
                if (x2 > x1) {
                    if (x4 > x1) {
                        return 1;
                    }
                } else {
                    if (x4 < x1) {
                        return 1;
                    }
                }
            } else if (x2 == x3) {
                if (x2 > x1) {
                    if (x4 < x2) {
                        return 1;
                    }
                } else {
                    if (x4 > x2) {
                        return 1;
                    }
                }               
            } else if (x1 == x4) {
                if (x2 > x1) {
                    if (x3 > x1) {
                        return 1;
                    }
                } else {
                    if (x3 < x1) {
                        return 1;
                    }
                }            
            } else if (x2 == x4) {
                if (x2 > x1) {
                    if (x3 < x2) {
                        return 1;
                    }
                } else {
                    if (x3 > x2) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}


int getIllegalFlag (std::vector<std::vector<std::pair<int, int>>> coords) {
    // if return 0, it means the generated street specification is valid;
    // if return 1, it means the generated street specification is invalid;
    // check: in each street, repetitive coords are not allowed (stronger restriction for input)
    for (const auto& streetCoords : coords) {
        std::vector<std::pair<int, int>> uniquePairs;
        for (const auto& coord : streetCoords) {
            if (std::find(uniquePairs.begin(), uniquePairs.end(), coord) != uniquePairs.end()) {
                return 1;
            } else {
                uniquePairs.push_back(coord);
            }
        }
    }    

    /*
    // check: in each street, intersective line segments are not allowed (stronger restriction for input)
    for (const auto& streetCoords : coords) {
        std::vector<std::vector<std::pair<int, int>>> lineSegCollect;
        for (int i = 0; i < streetCoords.size() - 1; ++i) {
            std::vector<std::pair<int, int>> lineSeg;
            lineSeg.push_back(streetCoords[i]);
            lineSeg.push_back(streetCoords[i + 1]);
            lineSegCollect.push_back(lineSeg);
        }

        if (lineSegCollect.size() > 1) {
            for (int i = 0; i < lineSegCollect.size() - 1; ++i) {
                if (intersectionCheck(lineSegCollect[i], lineSegCollect[i + 1]) == 1) {
                    illegalFlag = 1;
                    return illegalFlag;
                }
            }
        }
    }
    */

    // check: each 2 line segments are not allowed to overlap
    std::vector<std::vector<std::pair<int, int>>> lineSegCollect;
    for (const auto& streetCoords : coords) {
        for (int i = 0; i < (int) streetCoords.size() - 1; ++i) {
            std::vector<std::pair<int, int>> lineSeg;
            lineSeg.push_back(streetCoords[i]);
            lineSeg.push_back(streetCoords[i + 1]);
            lineSegCollect.push_back(lineSeg);
        }
    }

    for (int i = 0; i < (int) lineSegCollect.size(); ++i) {
        for (int j = i + 1; j < (int) lineSegCollect.size(); ++j) {
            if (overlapCheck(lineSegCollect[i], lineSegCollect[j]) == 1) {
                return 1;
            }
        }
    }    

    return 0;

}

int main(int argc, char **argv) {
    int option;
    int s = 10;
    int n = 5;
    int l = 5;
    int c = 20;

    int s_lo = 2;
    int n_lo = 1;
    int l_lo = 5;

    // store the generated number of streets, number of line segments(for each street), and wait time
    int streetNum = 0;
    std::vector<int> lineSegNum; 
    int waitTime = 0;

    // store the street names
    std::vector<std::string> streetNames;
    // store coordinates(for each street)
    std::vector<std::vector<std::pair<int, int>>> coords; 

    // record wrongAttempts for generating street specification
    int wrongAttemptCounter = 0;
    int attemptCeil = 25;
    

    // deal with the command line arguments
    while ((option = getopt(argc, argv, "s:n:l:c:")) != -1) {
        switch (option) {
            case 's':
                s = std::atoi(optarg);
                if (s < 2) {
                    std::cerr << "Error: The value for command line argument is out of range!" << std::endl;
                    // std::cout << "*" << std::endl;
                    // exit(1);
                    s = 10;
                }
                break;
            case 'n':
                n = std::atoi(optarg);
                if (n < 1) {
                    std::cerr << "Error: The value for command line argument is out of range!" << std::endl;
                    // exit(1);
                    n = 5;
                }
                break;
            case 'l':
                l = std::atoi(optarg);
                if (l < 5) {
                    std::cerr << "Error: The value for command line argument is out of range!" << std::endl;
                    // exit(1);
                    l = 5;
                }            
                break;
            case 'c':
                c = std::atoi(optarg);
                if (c < 1) {
                    std::cerr << "Error: The value for command line argument is out of range!" << std::endl;
                    // exit(1);
                    c = 20;
                }
                break;
            default:
                std::cerr << "Error: The given command is wrong!(a1)" << std::endl;
                // exit(1);
                break;
        }
    }

    // std::cout << "c equals: " << c << std::endl;


    while (true) {
        // remove existing streets
        // std::cout << "This C equals: " << c << std::endl;
        // std::cout << "Test: " << c << std::endl;
        // std::cout << "streetNum: " << streetNum << std::endl;
        
        /*
        for (int i = 0; i < streetNames.size(); ++i) {
            std::cout << streetNames[i] << " ";
        }
        std::cout << std::endl;
        */

        if (!streetNames.empty()) {
            for (const std::string& street : streetNames) {
                std::cout << "rm " << "\"" << street << "\"" << std::endl;
            }
            streetNames.clear();
        }

        streetNum = setStreetNum(s_lo, s);
        lineSegNum = setLineSegNum(streetNum, n_lo, n);
        waitTime = setWaitTime(l_lo, l);

        streetNames = setStreetNames(streetNum);
        
        coords = setCoords(lineSegNum, c);

        //printTest
        /*
        std::cout << "streetNum: " << streetNum << std::endl;
        
        for (int i = 0; i < streetNames.size(); ++i) {
            std::cout << streetNames[i] << " ";
        }
        std::cout << std::endl;
        

        //printTest
        // Print the elements

        for (const auto& streetCoods : coords) {
            std::cout << "Steet strarts: ---" << std::endl;
            for (const auto& coor : streetCoods) {
                std::cout << "Pair: (" << coor.first << ", " << coor.second << ")" << std::endl;
            }
            std::cout << "Steet ends: ---" << std::endl;
        }
        */
    

        if (getIllegalFlag(coords) == 1) {
            wrongAttemptCounter += 1;
            streetNames.clear();
            lineSegNum.clear();
            coords.clear();
            streetNum = 0;
            if (wrongAttemptCounter >= attemptCeil) {
                std::cerr << "Error: Failed to generate valid input for " << wrongAttemptCounter << " simultaneous attempts." << std::endl;
                exit(1);
            }
        } else {
            wrongAttemptCounter = 0; 
            for (int i = 0; i < streetNum; ++i) {
                std::cout << "add " << "\"" << streetNames[i] << "\" ";
                for (int j = 0; j < (int) coords[i].size(); ++j) {
                    std::cout << "(" << coords[i][j].first << "," << coords[i][j].second;
                    if (j < (int) coords[i].size() - 1) {
                        std::cout << ") ";
                    } else {
                        std::cout << ")";
                    }
                }
                std::cout << std::endl;
            }
            
            std::cout << "gg" << std::endl;
            sleep(waitTime);           
        }
    }
    
    return 0;
}

