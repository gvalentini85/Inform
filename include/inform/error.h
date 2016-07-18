// Copyright 2016 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#pragma once

#include <inform/export.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Error tags to be used in conjunction with `inform_error`
 */
typedef enum
{
    INFORM_SUCCESS      =  0, /// no error occurred
    INFORM_FAILURE      = -1, /// an unspecified error occurred
    INFORM_EFAULT       =  1, /// invalid pointer
    INFORM_EARG         =  2, /// invalid argument
    INFORM_ENOMEM       =  3, /// malloc/calloc/realloc failed
    INFORM_ETIMESERIES  =  4, /// time series is NULL
    INFORM_ENOINITS     =  5, /// time series has no initial conditions
    INFORM_ESHORTSERIES =  6, /// time series has less than two timesteps
    INFORM_EKZERO       =  7, /// history length is zero
    INFORM_EKLONG       =  8, /// history is too long for the time series
    INFORM_EBASE        =  9, /// the provided base is invalid
    INFORM_ENEGSTATE    = 10, /// time series has negative state
    INFORM_EBADSTATE    = 11, /// time series has states inconsistent with expected base
    INFORM_EDIST        = 12, /// invalid distribution
} inform_error;

/// set an error as pointed to by ERR
#define INFORM_ERROR(ERR, TAG) do {\
        if ((ERR) != NULL) {\
            *(ERR) = (inform_error) TAG;\
        }\
    } while(0)

/// set an error and return a result
#define INFORM_ERROR_RETURN(ERR, TAG, RET) do {\
        INFORM_ERROR(ERR, TAG); \
        return RET; \
    } while(0)

/// set an error and return a void result
#define INFORM_ERROR_RETURN_VOID(ERR, TAG) do {\
        INFORM_ERROR(ERR, TAG); \
        return; \
    } while(0)

EXPORT inline extern bool inform_succeeded(inform_error const *err);

EXPORT inline extern bool inform_failed(inform_error const *err);

#ifdef __cplusplus
}
#endif
