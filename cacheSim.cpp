#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "cache.h"

using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

int main(int argc, char **argv) {

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}

  Cache L1_cache(BSize , L1Size , L1Assoc);
  Cache L2_cache(BSize , L2Size , L2Assoc);
  double lines_counter = 0;
  double total_time = 0;
  double L1Acc = 0;
  double L2Acc = 0;
  double L1Miss = 0;
  double L2Miss = 0;

	while (getline(file, line)) {

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		// DEBUG - remove this line
		cout << "operation: " << operation;

		string cutAddress = address.substr(2); // Removing the "0x" part of the address

		// DEBUG - remove this line
		cout << ", address (hex)" << cutAddress;

		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);
    lines_counter += 1;

		// DEBUG - remove this line
		cout << " (dec) " << num << endl;

    //read Write proccess for each line:
    bool in_L1 = L1_cache.readWriteCache(num);
    total_time += L1Cyc;
    L1Acc += 1;
    if(in_L1){
      L1_cache.updateDirty(num);
      continue;
    }
    L1Miss += 1;
    bool in_L2 = L2_cache.readWriteCache(num);
    total_time += L2Cyc ;
    L2Acc += 1;
    address_t removed_address = 0;
    bool isDirty = false;
    if(in_L2){
      if ((operation == 'r') || (WrAlloc)){
        bool isRemoved = L1_cache.add2Cache(isDirty , removed_address , num);
        if (isRemoved && isDirty)
          L2_cache.updateLRU(removed_address);
        if (operation == 'w')
          L1_cache.updateDirty(num);
      }
    }
    else{
      total_time += MemCyc;
      L2Miss += 1;
      if ((operation == 'r') || (WrAlloc)){
        bool isRemoved = L2_cache.add2Cache(isDirty , removed_address , num);
        if(isRemoved)
          L1_cache.removeCache(removed_address);
        isRemoved = L1_cache.add2Cache(isDirty , removed_address , num);
        if (isRemoved && isDirty)
          L2_cache.updateLRU(removed_address);
        if (operation == 'w')
          L1_cache.updateDirty(num);
      }
    }
  }

  double L1MissRate = L1Miss / L1Acc;
  double L2MissRate = L2Miss / L2Acc;
	double avgAccTime = total_time / lines_counter;

	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
}
