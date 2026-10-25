#ifndef ERROR_H
#define ERROR_H


#include "hcae/error.h"


#define ERR_MSG_BUFSIZE 512


void set_err(HCAE_ErrorCode, const char*, void*, const char*, ...);


#endif