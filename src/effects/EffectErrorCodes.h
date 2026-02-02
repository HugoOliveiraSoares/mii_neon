#pragma once

#define EFFECT_SUCCESS 0

#define EFFECT_INVALID_NAME -1

#define EFFECT_NOT_INITIALIZED -5
#define EFFECT_INVALID_PARAMETER -6
#define EFFECT_OPERATION_FAILED -7

#define BRIGHTNESS_SUCCESS 0
#define BRIGHTNESS_OUT_OF_RANGE -3

#define BRIGHTNESS_MIN_VALUE 0
#define BRIGHTNESS_MAX_VALUE 255

#define ERROR_INVALID_NAME_STR "Invalid effect name specified"
#define ERROR_MEMORY_STR "Memory allocation failed"
#define ERROR_HARDWARE_STR "Hardware error occurred"
#define ERROR_NOT_INITIALIZED_STR "Effect system not initialized"
#define ERROR_INVALID_PARAM_STR "Invalid parameter provided"
#define ERROR_OP_FAILED_STR "Operation failed"

#define ERROR_BRIGHTNESS_INVALID_JSON_STR "Invalid JSON in brightness request"
#define ERROR_BRIGHTNESS_MISSING_STR "Brightness value is required"
#define ERROR_BRIGHTNESS_RANGE_STR "Brightness value must be between 0 and 128"
