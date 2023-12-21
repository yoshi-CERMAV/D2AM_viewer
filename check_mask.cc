//
//  check_mask.cc
//  D2AM2023
//
//  Created by yoshi on 31/07/2023.
//

#include "check_mask.h"
#include <iostream>
#include <fstream>
#include <list>
using namespace std;

void check_mask::reset(char *mask)
{
    for(int j = 1; j < dim11; j++){
       for(int i = 1; i < dim01; i++){
          int pos = j * dim0+i;
          if (mask[pos]){
             int neighbor=0;
             if(!mask[pos-1]) neighbor+=1;
             if(!mask[pos+1]) neighbor+=2;
             if(!mask[pos-dim0]) neighbor+=4;
             if(!mask[pos+dim0]) neighbor+=8;
             lis[neighbor].push_back(pos);
          }
       }
    }
    for(int i = 0; i < 16; i++){
       cout << i <<" "<<lis[i].size()<<endl;
    }
 #ifdef DUMP
    ofstream fo("bad_pixel");
    int endi = 0;
    for(int i = 0; i < 14; i++){
       int endi += lis[i+1].size;
       fo.write(reinterpret_cast<char *> (&endi), sizeof(int));
    }
 #endif

}
check_mask::check_mask(char *mask, size_t dim0_, size_t dim1_)
{
   dim0 = dim0_;
   dim1 = dim1_;
   dim01 = dim0_-1;
   dim11 = dim1_-1;
    reset(mask);
}

