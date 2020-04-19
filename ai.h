/*
 * Name: Siqi Zhou StudentId: 903274 UserName:szhou7
 * */
#ifndef __AI__
#define __AI__

#include <stdint.h>
#include <unistd.h>
#include "node.h"
#include "priority_queue.h"


void initialize_ai();

float get_reward( node_t* n );
move_t get_next_move( state_t init_state, int budget, 
                     propagation_t propagation, char* stats );
void propagateBackScoreToFirstAction(node_t** explored, 
                                     propagation_t propagation, 
                                     float **best_action_score, 
                                     unsigned explored_size);
void print_to_screen(unsigned max_depth, unsigned expanded_nodes, 
                     unsigned generated_nodes, char* stats, 
                     float* best_action_score, move_t best_action);
move_t find_best_move(float *best_action_score);
unsigned get_generated_nodes();
unsigned get_expanded_nodes();
unsigned get_max_depth();
#endif
