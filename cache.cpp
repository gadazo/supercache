#include <math.h>
#include "cache.h"
#include <vector>

using namespace std;

bool Cache :: readWriteCache(address_t address){
  int set_num = ((address/(int)pow(2,offset_bit_size)) & ((int)pow(2,set_bit_size)-1));
  int tag_num =  address/pow(2,offset_bit_size + set_bit_size);
  for (unsigned i = 0; i< num_of_ways ; i++){
    if((ways_array[i][set_num].tag = tag_num) && (ways_array[i][set_num].valid)){
      updateLRU(i , set_num);
      return true;
    }
  }
  return false ;
}

bool Cache :: add2Cache(bool& isDirty , address_t& removed_address , address_t added_address){
  int set_num = ((added_address/(int)pow(2,offset_bit_size)) & ((int)pow(2,set_bit_size)-1));
  int tag_num =  added_address/pow(2,offset_bit_size + set_bit_size);
  int way_num = -1;
  bool isRemoved = false;
  //check if there is an empty way (for the exact set)
    for (unsigned i = 0; i< num_of_ways ; i++){
      if(!ways_array[i][set_num].valid){
        way_num = i;
        break;
      }
    }
    if(way_num == -1){
      isRemoved = true;
      way_num = lru_array[set_num].back();
      lru_array[set_num].pop_back();
      isDirty = ways_array[way_num][set_num].isDirty;
      removed_address = ways_array[way_num][set_num].address;
    }
    lru_array[set_num].insert(lru_array[set_num].begin() ,way_num);
    ways_array[way_num][set_num].valid = true;
    ways_array[way_num][set_num].isDirty = false;
    ways_array[way_num][set_num].address = added_address;
    ways_array[way_num][set_num].tag = tag_num;
    return isRemoved;
}

void Cache :: removeCache (address_t address){
  int set_num = ((address/(int)pow(2,offset_bit_size)) & ((int)pow(2,set_bit_size)-1));
  int tag_num =  address/pow(2,offset_bit_size + set_bit_size);
  for (unsigned i = 0; i< num_of_ways ; i++){
    if((ways_array[i][set_num].tag = tag_num) && (ways_array[i][set_num].valid)){
      ways_array[i][set_num].valid = false;
      for (vector<int>::iterator it = lru_array[set_num].begin() ; it != lru_array[set_num].end() ; ++it){
        if (*it == (int)i)
          lru_array[set_num].erase(it);
      }
    }
  }
}
void Cache :: updateDirty (address_t address){
  int set_num = ((address/(int)pow(2,offset_bit_size)) & ((int)pow(2,set_bit_size)-1));
  int tag_num =  address/pow(2,offset_bit_size + set_bit_size);
  for (unsigned i = 0; i< num_of_ways ; i++){
    if((ways_array[i][set_num].tag = tag_num) && (ways_array[i][set_num].valid)){
      ways_array[i][set_num].isDirty = true;
    }
  }
}

void Cache :: updateLRU (address_t address){
  int set_num = ((address/(int)pow(2,offset_bit_size)) & ((int)pow(2,set_bit_size)-1));
  int tag_num =  address/pow(2,offset_bit_size + set_bit_size);
  for (unsigned i = 0; i< num_of_ways ; i++){
    if((ways_array[i][set_num].tag = tag_num) && (ways_array[i][set_num].valid)){
      updateLRU(i,set_num);
    }
  }
}

void Cache :: updateLRU (int way_num , int set_num){
  // remove the the way from the vector (if exsited)
  for (vector<int>::iterator it = lru_array[set_num].begin() ; it != lru_array[set_num].end() ; ++it){
    if (*it == way_num)
      lru_array[set_num].erase(it);
  }
  // insert the way to the front of the vector
  lru_array[set_num].insert(lru_array[set_num].begin() , way_num);
}
