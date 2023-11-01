#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

/*
Experiments to run:
1. Keep cache size constant by varying the associativity of the cache - 


*/

int main() {
    // Parameters to vary
    std::vector<int> num_sets_list = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    std::vector<int> num_blocks_list = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    std::vector<int> bytes_in_block_list = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    std::vector<std::string> write_policy_list = {"write-allocate", "no-write-allocate"};
    std::vector<std::string> write_through_or_back_list = {"write-back", "write-through"};
    std::vector<std::string> eviction_policy_list = {"lru", "fifo"};

    // File to write results
    std::ofstream outFile;
    outFile.open("4KB_analysis.csv");
    outFile << "Num_Sets,Num_Blocks,Bytes_in_Block,Write_Policy,Write_Through_or_Back,Eviction_Policy,Total_Loads,Total_Stores,Load_Hits,Load_Misses,Store_Hits,Store_Misses,Total_Cycles\n";

    for (int num_sets : num_sets_list) {
        for (int num_blocks : num_blocks_list) {
            for (int bytes_in_block : bytes_in_block_list) {
                for (std::string write_policy : write_policy_list) {
                    for (std::string write_through_or_back : write_through_or_back_list) {
                        for (std::string eviction_policy : eviction_policy_list) {

                            // check conditions
                            if (num_sets * num_blocks * bytes_in_block / 1000 != 4) {
                                continue;
                            }

                            // Execute the program with these parameters
                            std::string cmd = "./csim " + std::to_string(num_sets) + " " + std::to_string(num_blocks) + " " + std::to_string(bytes_in_block) + " " + write_policy + " " + write_through_or_back + " " + eviction_policy + " < gcc.trace";
                            std::string line;

                            // Assuming the output of cmd is redirected to a temporary file called "temp.txt"
                            cmd += " > temp.txt";
                            system(cmd.c_str());

                            // Now, read the output from temp.txt
                            std::ifstream inFile("temp.txt");
                            std::vector<std::string> results;

                            while (getline(inFile, line)) {
                                std::istringstream iss(line);
                                std::string word;

                                // Assuming each line has only one numeric result after the colon
                                while (iss >> word) {
                                    if (word.back() == ':') {
                                        iss >> word;
                                        results.push_back(word);
                                    }
                                }
                            }
                            inFile.close();

                            // Writing the results to the CSV
                            outFile << num_sets << "," << num_blocks << "," << bytes_in_block << "," << write_policy << "," << write_through_or_back << "," << eviction_policy;
                            for (std::string &result : results) {
                                outFile << "," << result;
                            }
                            outFile << "\n";
                        }
                    }
                }
            }
        }
    }
    outFile.close();
    return 0;
}
