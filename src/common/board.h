#ifndef BOARD_H
#define BOARD_H


#include "hcae/board.h"


typedef struct HCAE_CPU HCAE_CPU;


typedef enum BoardActivity {
    BRD_ACT_INACTIVE,
    BRD_ACT_RUNNING,
    BRD_ACT_PAUSED,
    BRD_ACT_FINISHEDRUN
} BoardActivity;


struct HCAE_Board {
    BoardActivity activity;

    HCAE_CPU* cpu;
};


#endif