#ifndef __HCAE_HEADER_CPU
#define __HCAE_HEADER_CPU


typedef struct HCAE_Board HCAE_Board;

typedef struct HCAE_CPU HCAE_CPU;
typedef struct HCAE_CPUModel HCAE_CPUModel;


#define HCAE_MAX_CORECNT 256
#define HCAE_MIN_CPUMODEL_NAME_LEN 3

#define HCAE_USE_DEFAULT_CORECNT ((size_t)-1)


HCAE_CPU* HCAE_CreateCPU(HCAE_Board*, HCAE_CPUModel*, size_t);
int HCAE_DestroyCPU(HCAE_CPU*);

HCAE_CPUModel* HCAE_CreateCPUModel(const char*);
int HCAE_DestroyCPUModel(HCAE_CPUModel*);

int HCAE_SetCPUModelMaximumCoreCount(HCAE_CPUModel*, size_t);
int HCAE_SetCPUModelDefaultCoreCount(HCAE_CPUModel*, size_t);

int HCAE_IsCPUModelNameValid(const char*);


#endif