#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "common/error.h"


static _Thread_local HCAE_ErrorCode err_code; // error code
static _Thread_local const char* err_funcname; // the name of the function that caused the error
static _Thread_local void* err_obj;
static _Thread_local char err_msg[ERR_MSG_BUFSIZE]; // the error message describing the error


void set_err(HCAE_ErrorCode code, const char* funcname, void* obj, const char* msg_fstr, ...) {
    err_code = code;
    err_funcname = funcname;
    err_obj = obj;

    if (msg_fstr) {
        va_list args;
        va_start(args, msg_fstr);
        vsnprintf(err_msg, sizeof(err_msg), msg_fstr, args);
        va_end(args);
    } else {
        memset(err_msg, 0, sizeof(err_msg));    
    }
}


HCAE_ErrorCode HCAE_GetErrorCode(void) {
    return err_code;
}

const char* HCAE_GetErrorFunctionName(void) {
    return err_funcname;
}

const char* HCAE_GetErrorMessage(void) {
    return err_msg;
}

void* HCAE_GetErrorObject(void) {
    return err_obj;
}


void HCAE_ClearError(void) {
    err_code = HCAE_ERRCODE_NONE;
    err_funcname = NULL;
    err_obj = NULL;
    memset(err_msg, 0, sizeof(err_msg));
}

