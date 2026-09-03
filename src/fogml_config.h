/*
   Copyright 2026 FogML
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "fogml.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define FOGML_VERBOSE

// DIGITAL SIGNAL PROCESSING
// number of features - depends on the DSP blocks
#define FOGML_VECTOR_SIZE 16


// RESERVOIR SAMPLING
#define MY_RESERVOIR_SIZE  10
float my_reservoir[MY_RESERVOIR_SIZE * FOGML_VECTOR_SIZE];

tinyml_reservoir_sampling_config_t my_rs_config = {
  .n = MY_RESERVOIR_SIZE,
  .k = 0,
  .vector_size = FOGML_VECTOR_SIZE,
  .reservoir = my_reservoir
};


// LOCAL OUTLINER FACTOR
float my_kdistance[MY_RESERVOIR_SIZE];
float my_lrd[MY_RESERVOIR_SIZE];

tinyml_lof_config_t my_lof_config = {
  .parameter_k = 2,  //k nearest neighbours are considered
  .k_distance = my_kdistance,  //table of k-distance for each of n points
  .lrd = my_lrd, //Local Reachability Density for each of n points
  .n = MY_RESERVOIR_SIZE, //number of points in data
  .vector_size = FOGML_VECTOR_SIZE, //dimension of each points
  .data = my_reservoir //set of points
};


void fogml_learning(float *vector) {
#ifdef FOGML_VERBOSE
    fogml_printf("Updating reservoir.");
#endif
   
    tinyml_reservoir_sampling(vector, &my_rs_config);

#ifdef FOGML_VERBOSE
    tinyml_reservoir_verbose(&my_rs_config);
    for(int i = 0; i < FOGML_VECTOR_SIZE; i++) {
        fogml_printf_float(vector[i]);
        fogml_printf(" ");
    }
    fogml_printf("\n");
#endif

    //If the number of points in the reservoir is lower than its capacity then LOF should analyse only the available points
    if (my_rs_config.k < MY_RESERVOIR_SIZE){
        my_lof_config.n = my_rs_config.k;
    } else{
        my_lof_config.n = MY_RESERVOIR_SIZE;
    }
    tinyml_lof_learn(&my_lof_config);

    //free(vector);
}


void fogml_processing(float *vector, float *score) {

#ifdef FOGML_VERBOSE
    for(int i = 0; i < FOGML_VECTOR_SIZE; i++) {
        fogml_printf_float(vector[i]);
        fogml_printf(" ");
    }
    fogml_printf("\n");
#endif

    *score = tinyml_lof_score(vector, &my_lof_config);

#ifdef FOGML_VERBOSE
    fogml_printf("LOF Score = ");
    fogml_printf_float(*score);
    fogml_printf("\n");
#endif

    //free(vector);
} 

#ifdef __cplusplus
} // extern "C"
#endif
