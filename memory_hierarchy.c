/*************************************************************************************|
|   1. YOU ARE NOT ALLOWED TO SHARE/PUBLISH YOUR CODE (e.g., post on piazza or online)|
|   2. Fill main.c and memory_hierarchy.c files                                       |
|   3. Do not use any other .c files neither alter main.h or parser.h                 |
|   4. Do not include any other library files                                         |
|*************************************************************************************/
#include "mipssim.h"

/// @students: declare cache-related structures and variables here

struct Block {
    int valid_bit;
    int tag;
    int data;
};

struct Cache {
    int reading, writing;
    int block_size;
    int noOfLines;
    struct Block *blocks;
};

struct Cache *myCache;
struct memory_stats_t *myStats;
struct Block create_a_block(){
 struct Block *size = malloc(16);
 return *size;
}
void memory_state_init(struct architectural_state* arch_state_ptr) {
    arch_state_ptr->memory = (uint32_t *) malloc(sizeof(uint32_t) * MEMORY_WORD_NUM);
    memset(arch_state_ptr->memory, 0, sizeof(uint32_t) * MEMORY_WORD_NUM);
    if(cache_size == 0){
        // CACHE DISABLED
        memory_stats_init(arch_state_ptr, 0); // WARNING: we initialize for no cache 0
    }else {
        // CACHE ENABLED
        int index_size = 0;
        if (cache_size > 0){
            while (cache_size % 2 == 0) {
                cache_size/=2;
                index_size ++;
            }
            if (cache_size == 0 || cache_size != 1){
                printf ("%d is not a power of 2!\n");
            }
        }
    myCache = malloc (sizeof(uint64_t)*cache_size);
    myStats = malloc (sizeof(uint64_t)*2);
    myCache -> blocks = malloc (sizeof(uint32_t)*16);
    myCache->block_size=16;
    myCache->noOfLines = (int)(cache_size/myCache->block_size);
    myCache->reading = 0;
    myCache->writing = 0;
    myStats->lw_cache_hits =0;
    myStats->lw_total=0;
    myStats->sw_cache_hits=0;
    myStats->sw_total=0;
    
    for (int i = 1; i<cache_size/16; i++){
        myCache->blocks[i] = create_a_block();
        myCache->blocks[i].valid_bit = 0;
        myCache->blocks[i].tag = 0;
        myCache->blocks[i].data = 0;

    }
        /// @students: memory_stats_init(arch_state_ptr, X); <-- fill # of tag bits for cache 'X' correctly
    memory_stats_init(arch_state_ptr,32-index_size-4);
    }
}


// returns data on memory[address / 4]
int memory_read(int address){
    arch_state.mem_stats.lw_total++;
    check_address_is_word_aligned(address);

    if(cache_size == 0){
        // CACHE DISABLED
        return (int) arch_state.memory[address / 4];
    }else{
        
        // CACHE ENABLED
    if (arch_state.control.MemRead){
        //int offset = get_piece_of_a_word(address, 0, 10);

        int mem_tag = get_piece_of_a_word(address,11,21);
        int idx = get_piece_of_a_word(address,4,8);
        if (myCache->blocks[idx].tag == mem_tag && myCache->blocks[idx].valid_bit == 1){
            myStats->lw_total ++;
            myStats->lw_cache_hits ++;
        } else {
            memory_write(arch_state.memory[address / 4], mem_tag);
            myCache->blocks[idx].data = address;
            myCache->blocks[idx].valid_bit =1;
            myCache->blocks[idx].tag = mem_tag; 
        }
    printf("lw hits are %d\n", myStats->lw_cache_hits);
    printf("lw amount is %d\n", myStats->lw_total);

    }
        /// @students: your implementation must properly increment: arch_state_ptr->mem_stats.lw_cache_hits
    }
    return 0;
}

// writes data on memory[address / 4]
void memory_write(int address, int write_data){
    arch_state.mem_stats.sw_total++;
    check_address_is_word_aligned(address);

    if(cache_size == 0){
        // CACHE DISABLED
        arch_state.memory[address / 4] = (uint32_t) write_data;
    }else{
        // CACHE ENABLED
        if (arch_state.control.MemWrite){
            int mem_tag = get_piece_of_a_word(address,11,21);
            int idx = get_piece_of_a_word(address,4,8);
            if (myCache->blocks[idx].tag == mem_tag && myCache->blocks[idx].valid_bit == 1){
            myStats->sw_total ++;
            myStats->sw_cache_hits ++;
            arch_state.memory[address / 4] = (uint32_t) write_data;
            myCache->blocks[idx].data = address;
            myCache->blocks[idx].valid_bit =1;
            myCache->blocks[idx].tag = mem_tag; 
        } else {
            arch_state.memory[address / 4] = (uint32_t) write_data;
        }
        /// @students: your implementation must properly increment: arch_state_ptr->mem_stats.sw_cache_hits
    }
}
}
