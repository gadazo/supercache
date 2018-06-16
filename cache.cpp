#include <math.h>
#include "cache.h"
#include <vector>
#include <iostream>
using namespace std;

bool Cache::readWriteCache(address_t address) {
	//the Read/Write function - main function is to check if in cache and to update LRU if exist
	int set_num = ((address / (int)pow(2, offset_bit_size)) & ((int)pow(2, set_bit_size) - 1));
	int tag_num = address / pow(2, offset_bit_size + set_bit_size);

	for (unsigned i = 0; i < num_of_ways; i++) {
		if ((ways_array[i][set_num].valid) && (ways_array[i][set_num].tag == tag_num)) {
			// if exist
			updateLRU(i, set_num);
			return true;
		}
	}
	return false;
}

bool Cache::add2Cache(bool& isDirty, address_t& removed_address, address_t added_address) {
	//adds the data to the cache - check if there is no empty space evict by LRU .
	//outputs : if data was removed(bool), if it was dirty(isDirty) and it's address(removed_address)
	int set_num = ((added_address / (int)pow(2, offset_bit_size)) & ((int)pow(2, set_bit_size) - 1));
	int tag_num = added_address / pow(2, offset_bit_size + set_bit_size);
	int way_num = -1;
	bool isRemoved = false;
	//check if there is an empty way (for the exact set)
	for (unsigned i = 0; i < num_of_ways; i++) {
		if (!ways_array[i][set_num].valid) {
			way_num = i;
			break;
		}
	}
	if (way_num == -1) {
		//there is no empty way evict by the LRU
		isRemoved = true;
		way_num = lru_array[set_num].back();
		lru_array[set_num].pop_back();
		isDirty = ways_array[way_num][set_num].isDirty;
		removed_address = ways_array[way_num][set_num].address;
	}
	//adds the new way in the LRU and add the new data
	lru_array[set_num].insert(lru_array[set_num].begin(), way_num);
	ways_array[way_num][set_num].valid = true;
	ways_array[way_num][set_num].isDirty = false;
	ways_array[way_num][set_num].address = added_address;
	ways_array[way_num][set_num].tag = tag_num;
	return isRemoved;
}

void Cache::removeCache(address_t address) {
	//remove data from cache empty the way from the LRU
	int set_num = ((address / (int)pow(2, offset_bit_size)) & ((int)pow(2, set_bit_size) - 1));
	int tag_num = address / pow(2, offset_bit_size + set_bit_size);
	for (unsigned i = 0; i < num_of_ways; i++) {
		if ((ways_array[i][set_num].valid) && (ways_array[i][set_num].tag == tag_num)) {
			ways_array[i][set_num].valid = false;
			for (vector<int>::iterator it = lru_array[set_num].begin(); it != lru_array[set_num].end(); ++it) {
				if (*it == (int)i) {
					lru_array[set_num].erase(it);
					break;
				}
			}
		}
	}
}
void Cache::updateDirty(address_t address) {
	// update the specific instance for beening dirty.
	//     used only for L1 cache and if L2 LRU needs an update
	int set_num = ((address / (int)pow(2, offset_bit_size)) & ((int)pow(2, set_bit_size) - 1));
	int tag_num = address / pow(2, offset_bit_size + set_bit_size);
	for (unsigned i = 0; i < num_of_ways; i++) {
		if ((ways_array[i][set_num].valid) && (ways_array[i][set_num].tag == tag_num)) {
			ways_array[i][set_num].isDirty = true;
		}
	}
}

void Cache::updateLRU(address_t address) {
	//public function: update the LRU by address.
	int set_num = ((address / (int)pow(2, offset_bit_size)) & ((int)pow(2, set_bit_size) - 1));
	int tag_num = address / pow(2, offset_bit_size + set_bit_size);
	for (unsigned i = 0; i < num_of_ways; i++) {
		if ((ways_array[i][set_num].valid) && (ways_array[i][set_num].tag == tag_num)) {
			updateLRU(i, set_num);
		}
	}
}

void Cache::updateLRU(int way_num, int set_num) {
	//pirvate function : update the LRU by set and way
	// remove the the way from the vector (if exsited)
	if (!lru_array[set_num].empty()) {
		for (vector<int>::iterator it = lru_array[set_num].begin(); it != lru_array[set_num].end(); ++it) {
			if (*it == way_num) {
				lru_array[set_num].erase(it);
				break;
			}
		}
	}
	// insert the way to the front of the vector
	lru_array[set_num].insert(lru_array[set_num].begin(), way_num);
}
