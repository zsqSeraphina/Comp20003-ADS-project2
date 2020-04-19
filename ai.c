/*
 * Name: Siqi Zhou StudentId: 903274 UserName:szhou7
 * */

#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>


#include "ai.h"
#include "utils.h"
#include "priority_queue.h"


struct heap h;


unsigned record_generated_nodes = 0;
unsigned record_expanded_nodes = 0;
unsigned record_max_depth = 0;

/**
 * Function called by pacman.c
*/
void initialize_ai(){
    heap_init(&h);
}

/**
 * function to copy a src into a dst state
*/
void copy_state(state_t* dst, state_t* src){
    //Location of Ghosts and Pacman
    memcpy( dst->Loc, src->Loc, 5*2*sizeof(int) );

    //Direction of Ghosts and Pacman
    memcpy( dst->Dir, src->Dir, 5*2*sizeof(int) );

    //Default location in case Pacman/Ghosts die
    memcpy( dst->StartingPoints, src->StartingPoints, 5*2*sizeof(int) );

    //Check for invincibility
    dst->Invincible = src->Invincible;
    
    //Number of pellets left in level
    dst->Food = src->Food;
    
    //Main level array
    memcpy( dst->Level, src->Level, 29*28*sizeof(int) );

    //What level number are we on?
    dst->LevelNumber = src->LevelNumber;
    
    //Keep track of how many points to give for eating ghosts
    dst->GhostsInARow = src->GhostsInARow;

    //How long left for invincibility
    dst->tleft = src->tleft;

    //Initial points
    dst->Points = src->Points;

    //Remiaining Lives
    dst->Lives = src->Lives;   

}

node_t* create_init_node( state_t* init_state ){
    node_t* new_n = (node_t*) malloc(sizeof(node_t));
    assert(new_n != NULL);
    new_n->parent = NULL;
    new_n->priority = 0;
    new_n->depth = 0;
    new_n->num_childs = 0;
    copy_state(&(new_n->state), init_state);
    new_n->acc_reward = get_reward( new_n );
    return new_n;
}


float heuristic( node_t* n ){
    float h = 0;
    float i = 0;
    float l = 0;
    float g = 0;
    if(n->parent != NULL){
        //when the pacman is invincible at this node
        //but not at the last node, it should be just ate a fruit
        if(n->state.Invincible == 1 && n->parent->state.tleft == 0){
            i = 10;
        }
        //when pacman at a node is not dead but its live is less than parent
        if((n->state.Lives > -1)&&(n->parent->state.Lives > n->state.Lives)){
            l = 10;
        //if lives==-1, pacman at this node is dead
        }else if(n->state.Lives == -1){
            g = 100;
        }
    }
    h = i - l - g;
    return h;
}

float get_reward ( node_t* n ){
    float reward = 0;
    float h = heuristic(n);
    float score = n->state.Points;
    float parent_score;
    //if the node is the first one, parent score
    //should just be its score
    if(n->parent == NULL){
        parent_score = score;
    }else{
        parent_score = n->parent->state.Points;
    }
    float discount = pow(0.99,n->depth);
    reward = h + score - parent_score;
    return discount * reward;
}

/**
 * Apply an action to node n and return a 
 *new node resulting from executing the action
*/
bool applyAction(node_t* n, node_t** new_node, move_t action ){
    bool changed_dir = false;
    //copy the data from the popped out prior node to a new node
    *new_node = (node_t*)malloc(sizeof(node_t));
    assert(*new_node != NULL);
    copy_state(&((*new_node)->state), &(n->state));
    changed_dir = execute_move_t( &((*new_node)->state), action );
    (*new_node)->parent = n;
    (*new_node)->depth = n->depth + 1;
    (*new_node)->priority = -((*new_node)->depth);
    (*new_node)->num_childs = 0;      //a new node has no childs
    //if the new node is one of the first 4, its move should just
    //be the action it is testing, and it has no parent reward to accumulate
    if((*new_node)->depth == 1){
        (*new_node)->move = action;
        (*new_node)->acc_reward=get_reward(*new_node);
    }
    //if new node is not one of the first 4, 
    //its move will be the move of its parent, 
    //same as one of the first 4 where this new node generated from, 
    //and it needs to add its parent's reward
    else{
        (*new_node)->move = n->move;
        (*new_node)->acc_reward=
            get_reward(*new_node)+(*new_node)->parent->acc_reward;
    }
    
    return changed_dir;   //determine if there is a wall in that direction

}


/**
 * Find best action by building all possible paths up to budget
 * and back propagate using either max or avg
 */

move_t get_next_move( state_t init_state, int budget, 
                     propagation_t propagation, char* stats ){
    unsigned generated_nodes = 0;
    unsigned expanded_nodes = 0;
    unsigned max_depth = 0;
    unsigned explored_size = 0;
    bool check_action;
    
    //Add the initial node
    node_t* n = create_init_node( &init_state );
    
    //initialise the empty explored array
    node_t** explored  = (node_t**)malloc(sizeof(node_t*));
    assert(explored != NULL);
    
    //Use the max heap API provided in priority_queue.h
    heap_push(&h,n);
    
    node_t* prior_node = NULL;
    
    //while the PQ is not empty
    while(h.count>0){
        //pop the node with highest priority out
        prior_node = heap_delete(&h);
        //check the space of explored, if not enough, 
        //realloc one more and add the popped node into explored
        if(explored_size > 0){
            node_t** tmp = realloc(explored, 
                                   sizeof(node_t*)*(explored_size+1));
            assert(tmp != NULL);
            explored = tmp;
        }
        memcpy(&explored[explored_size], &prior_node, sizeof(node_t*));
        explored_size++;
        expanded_nodes++;
        //if size of explored < budget
        if(explored_size < budget){
            //for each applicable action
            for(int dir = 0; dir < 4; dir++){
                node_t* new_node = NULL;
                //check if this move is valid
                //newNode<-applyAction
                check_action = applyAction(prior_node, &new_node, dir);
                if(check_action){
                    //update the max_depth
                    //new node depth always larger or equal to max depth
                    max_depth = new_node->depth;
                    prior_node->num_childs++;
                    generated_nodes++;
                    //if not lose life, add to frontier
                    if(new_node->state.Lives >= prior_node->state.Lives){
                        heap_push(&h, new_node);
                        
                    }
                    //if lose life, delete node
                    else{
                        free(new_node);
                    }
                }
                //if move invalid, delete node
                else{
                    free(new_node);
                }
            }
        }
    }
    //select best action breaking ties randomly
    move_t best_action;
    float* best_action_score = NULL;
    //to find out the highest score in each direction
    propagateBackScoreToFirstAction(explored, propagation, 
                                    &best_action_score, explored_size-1);
    //compare the highest score in each direction and find the
    //highest one, if tie exist, break randomly
    best_action = find_best_move(best_action_score);
    print_to_screen(max_depth, expanded_nodes, generated_nodes, 
                    stats, best_action_score, best_action);
    //get the max_depth, generated_nodes, expanded_nodes to 
    //print into the output.txt
    record_max_depth = max_depth;
    record_generated_nodes = generated_nodes;
    record_expanded_nodes = expanded_nodes;
    //free memory
    free(explored);
    free(best_action_score);
    emptyPQ(&h);
    return best_action;
}


/*
*find out the highest score in each direction
*/
void propagateBackScoreToFirstAction(node_t** explored, 
                                     propagation_t propagation, 
                                     float **best_action_score, 
                                     unsigned explored_size){
    
    //count how many nodes exist in each direction
    int node_count[4];
    for(int i = 0; i < 4; i++){
        node_count[i] = 0;
    }
    //to collect the highest score
    float choose_action[4];
    for(int i = 0; i < 4; i++){
        choose_action[i] = 0;
    }
    
    
    //go to the end of queue to and compare from the end to the start
    for(int i = explored_size; i >= 0; i--){
        node_t* curr_node = explored[i];
        //a node with num_childs==0 mean it is the most bottom of the tree
        if(curr_node->num_childs == 0){
            //for max propagation, compare the reward 
            //and leave the larger one with the corresponding direction
            if(propagation == max){
                if(choose_action[curr_node->move] <= curr_node->acc_reward){
                    choose_action[curr_node->move] = curr_node->acc_reward;
                }
            }
            //for avg propagation, add each leaf node together
            else if(propagation == avg){
                choose_action[curr_node->move] += curr_node->acc_reward;
            }
            //count the sum of leaf nodes in each direction
            node_count[curr_node->move] ++;
        }
        free(curr_node);
    }
    //devide the sum of rewards by the number of leaf nodes in each direction
    if(propagation == avg){
        for (int i = 0; i < 4; i++){
            if(node_count[i]!=0){
                choose_action[i] = choose_action[i]/node_count[i];
            }
        }
    }
    //change the direction that is unavailable(there is a wall) into INT_MIN
    for(int i = 0; i < 4; i++){
        if(node_count[i] == 0){
            choose_action[i] = INT_MIN;
        }
    }
    //copy the counted array to the array passed in
    *best_action_score = malloc(sizeof(float)*4);
    assert(*best_action_score != NULL);
    memcpy(*best_action_score, choose_action, sizeof(float)*4);
}



/*find out the move with highest reward and break tie randomly
*/
move_t find_best_move(float* best_action_score){
    move_t best_action = left;
    float best_score = -1;
    int tie_num = 0;
    int* tie_move = NULL;
    //find the largest reward
    for(int i = 0; i < 4; i++){
        if(best_score < best_action_score[i]){
            best_score = best_action_score[i];
            best_action = i;
        }
    }
    //if there is more than 1 highest score, there is a tie
    for(int i = 0; i < 4; i++){
        if(best_score == best_action_score[i]){
            //check if there is enough space in tie_move and allocate if not
            if(tie_num == 0){
                tie_move = (int*)malloc(sizeof(int));
                assert(tie_move != NULL);
                tie_move[tie_num] = i;
            }else{
                int* tmp = realloc(tie_move, sizeof(int)*(tie_num+1));
                assert(tmp != NULL);
                tie_move =  tmp;
                tie_move[tie_num] = i;
            }
            //count the size of tie_move
            tie_num++;
        }
    }
    //randomly choose a index of tie_move and therefore
    //randomly choose a move
    if(tie_move !=  NULL && tie_num > 0){
        tie_num = rand()%(tie_num);
        best_action = tie_move[tie_num];
    }
    free(tie_move);
    return best_action;
}


/*copied from base code, print some information to the screen*/
void print_to_screen(unsigned max_depth, unsigned expanded_nodes, 
                     unsigned generated_nodes, char* stats, float* 
                     best_action_score, move_t best_action){
    sprintf(stats, "Max Depth: %d Expanded nodes: %d  Generated nodes: %d\n",
    max_depth,expanded_nodes,generated_nodes);

    if(best_action == left)
        sprintf(stats, "%sSelected action: Left\n",stats);
    if(best_action == right)
        sprintf(stats, "%sSelected action: Right\n",stats);
    if(best_action == up)
        sprintf(stats, "%sSelected action: Up\n",stats);
    if(best_action == down)
        sprintf(stats, "%sSelected action: Down\n",stats);
        
    sprintf(stats, "%sScore Left %f Right %f Up %f Down %f",
    stats,best_action_score[left],best_action_score[right],
    best_action_score[up],best_action_score[down]);
}

//pass the generated_nodes to pacman.c for print to file
unsigned get_generated_nodes(){
    return record_generated_nodes;
}
//pass the expanded_nodes to pacman.c for print to file
unsigned get_expanded_nodes(){
    return record_expanded_nodes;
}

//pass the max_depth to pacman.c for print to file
unsigned get_max_depth(){
    return record_max_depth;
}