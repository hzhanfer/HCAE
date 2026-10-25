#ifndef __HCAE_HEADER_BOARD
#define __HCAE_HEADER_BOARD


typedef struct HCAE_Board HCAE_Board;


HCAE_Board* HCAE_CreateBoard(void);
int HCAE_DestroyBoard(HCAE_Board*);

int HCAE_IsBoardActive(HCAE_Board*);


#endif