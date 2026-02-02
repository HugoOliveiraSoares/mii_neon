#pragma once

// Error code constants for better error handling
// Replace magic numbers with meaningful constants

// Success codes
#define EFFECT_SUCCESS             0
#define EFFECT_SET_SUCCESS         0

// Error codes  
#define EFFECT_INVALID_NAME       -1
#define EFFECT_SET_FAILED         -1
#define EFFECT_ENUM_INVALID       -1

// ESP8266 specific error codes
#define EFFECT_MEMORY_ERROR      -2
#define EFFECT_HARDWARE_ERROR    -3
#define EFFECT_INITIALIZATION_FAILED -4

// Operational error codes
#define EFFECT_NOT_INITIALIZED   -5
#define EFFECT_INVALID_PARAMETER -6
#define EFFECT_OPERATION_FAILED  -7

// String representations of errors (for debugging)
#define ERROR_INVALID_NAME_STR    "Invalid effect name specified"
#define ERROR_MEMORY_STR          "Memory allocation failed"
#define ERROR_HARDWARE_STR        "Hardware error occurred"
#define ERROR_NOT_INITIALIZED_STR "Effect system not initialized"
#define ERROR_INVALID_PARAM_STR   "Invalid parameter provided"
#define ERROR_OP_FAILED_STR       "Operation failed"