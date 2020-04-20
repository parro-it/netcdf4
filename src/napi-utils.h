#define NAPI_EXPERIMENTAL 1
#include <node_api.h>

#define SUPPRESS_UNUSED_WARNING(x) (void)(x)

#define SECOND_ARG(_, ARG, ...) ARG

#define INVALID_C_VALUE_ERR(target, frmt_char, ...)                                                \
	{                                                                                              \
		const char *frmt = "Invalid value `" frmt_char "`: %s";                                         \
		int error_len = snprintf(NULL, 0, frmt, __VA_ARGS__);                                      \
		char *error_buff = malloc(error_len + 1);                                                  \
		snprintf(error_buff, error_len + 1, frmt, __VA_ARGS__);                                    \
		free(SECOND_ARG(__VA_ARGS__));                                                             \
		target = error_buff;                                                                       \
	}

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif


  napi_value i32_to_value(napi_env env, int32_t c_value, char **error);
  int32_t value_to_i32(napi_env env, napi_value value, char **error);
  napi_value utf8_to_value(napi_env env, char *c_value, char **error);
  char *value_to_utf8(napi_env env, napi_value value, char **error);

	napi_value nuts_u64_to_value(napi_env env, uint64_t c_value, char **error);
	napi_value nuts_u32_to_value(napi_env env, uint32_t c_value, char **error);
	napi_value nuts_u16_to_value(napi_env env, uint16_t c_value, char **error);
	napi_value nuts_u8_to_value(napi_env env, uint8_t c_value, char **error);
	napi_value nuts_i64_to_value(napi_env env, int64_t c_value, char **error);
	napi_value nuts_i16_to_value(napi_env env, int16_t c_value, char **error);
	napi_value nuts_i8_to_value(napi_env env, int8_t c_value, char **error);
	napi_value nuts_double_to_value(napi_env env, double c_value, char **error);
	napi_value nuts_float_to_value(napi_env env, float c_value, char **error);


  // return a js error based on the last napi error occurred.
  napi_value create_napi_error(napi_env env, const napi_extended_error_info *napi_err);


  // return NULL if status_to_check is napi_ok, or the exception
  // occurred if there is a pending one, or an error describing
  // the napi error occurred.
  napi_value error_for_napi_status(napi_env env, napi_status status_to_check);


#ifdef __cplusplus
}
#endif

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

#define DECLARE_NAPI_PROP(name, getter, setter)                          \
{ name, 0, 0, getter, setter, 0, napi_default, 0 }


// call a napi function and if status is not ok,
// throw an exception and return NULL
#define NAPI_CALL_BASE(FN_CALL, ON_ERR, RET_VAL) do {               \
  napi_value error;                                                 \
  napi_status status = (FN_CALL);                                   \
  if (NULL != (error = error_for_napi_status(env, status))) {       \
    ON_ERR;                                                         \
    return RET_VAL;                                                 \
  }                                                                 \
} while (0)                                                         \

// call a napi function and if returned status is not ok,
// throw an exception and return NULL
#define NAPI_CALL(FN_CALL) NAPI_CALL_BASE(FN_CALL, napi_throw(env, error), NULL)

// define an exported function
#define NUT_FN(NAME) {                                                                    \
    napi_value func;                                                                      \
	NAPI_CALL(napi_create_function(env, #NAME, NAPI_AUTO_LENGTH, NAME, NULL, &func));       \
	NAPI_CALL(napi_set_named_property(env, exports, #NAME, func));                          \
}

#define NUT_DEF_FN(NAME)                                                                  \
	napi_value NAME(napi_env env, napi_callback_info info)

// define a module
#define NUT_MODULE(NAME, INIT_BLOCK)                                                      \
	napi_value nut_##NAME##_module(napi_env env, napi_value exports) {                      \
		INIT_BLOCK                                                                            \
		return NULL;                                                                          \
	}                                                                                       \
                                                                                          \
	NAPI_MODULE(NAME, nut_##NAME##_module)

// arguments declaration

#define _INC_ARG_IDX() arg_idx++;
#define _READARG_0(ARG, ...) ARG _INC_ARG_IDX() _READARG_1(__VA_ARGS__)
#define _READARG_1(ARG, ...) ARG _INC_ARG_IDX() _READARG_2(__VA_ARGS__)
#define _READARG_2(ARG, ...) ARG _INC_ARG_IDX() _READARG_3(__VA_ARGS__)
#define _READARG_3(ARG, ...) ARG _INC_ARG_IDX() _READARG_4(__VA_ARGS__)
#define _READARG_4(ARG, ...) ARG _INC_ARG_IDX() _READARG_5(__VA_ARGS__)
#define _READARG_5(ARG, ...) ARG _INC_ARG_IDX() _READARG_6(__VA_ARGS__)
#define _READARG_6(ARG, ...) ARG


/**
 *
 * ARGS macro declare name and types of arguments
 * in napi function.
 *
 */
#define OPTIONAL_ARGS(OPT_N, N, ...)                                                    \
	napi_value argv[N];                                                                	\
	size_t argc = N;                                                                   	\
	napi_status status;                                                                	\
	char* nuts_error = NULL;															\
	SUPPRESS_UNUSED_WARNING(status);                                                   	\
	SUPPRESS_UNUSED_WARNING(nuts_error);                                               	\
	size_t arg_idx = 0;                                                                	\
    napi_value jsthis;                                                                  \
	napi_get_cb_info(env, info, &argc, argv, &jsthis, NULL);                           	\
	if ((long)argc < (N - OPT_N) || argc > N) {                                               \
		NAPI_CALL(napi_throw_error(env, NULL, "Wrong number of arguments."));          	\
		return NULL;                                                                   	\
	} 				                                                                   	\
	_READARG_0(__VA_ARGS__)

#define ARGS(N, ...) OPTIONAL_ARGS(0, N, __VA_ARGS__)


#define _CHECK_NUTS_ERROR 																\
	if (nuts_error != NULL) {															\
		napi_throw_error(env, NULL, nuts_error);										\
		free(nuts_error);                                   							\
		return NULL;																	\
	}

#define VAR_I32_FROM_JS(VAR_NAME, VALUE_JS) 											\
	int32_t VAR_NAME = 0;																\
	if (VALUE_JS != NULL) {																\
	    VAR_NAME= value_to_i32(env, VALUE_JS, &nuts_error); 							\
	    _CHECK_NUTS_ERROR																\
	}

#define VAR_STR_FROM_JS(VAR_NAME, VALUE_JS) 											\
	char * VAR_NAME = NULL;										 						\
	if (VALUE_JS != NULL) {																\
	    VAR_NAME= value_to_utf8(env, VALUE_JS, &nuts_error); 							\
	    _CHECK_NUTS_ERROR																\
	}



/**
 * @brief Converts a napi_value to an int64_t.
 *
 * Converts a napi_value containing a Number or a BigInt in an int64_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a BigInt not between C INT64_MIN and INT64_MAX
 *
 * Any valid value is converted to an int64_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
int64_t nuts_value_to_i64(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to an int32_t.
 *
 * Converts a napi_value containing a Number in an int32_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a Number or BigInt not between C INT32_MIN and INT32_MAX
 *
 * Any valid value is converted to an int32_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
int32_t nuts_value_to_i32(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to an int16_t.
 *
 * Converts a napi_value containing a Number in an int16_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a Number or BigInt not between C INT16_MIN and INT16_MAX
 *
 * Any valid value is converted to an int16_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
int16_t nuts_value_to_i16(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to an int8_t.
 *
 * Converts a napi_value containing a Number in an int8_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a Number or BigInt not between C INT8_MIN and INT8_MAX
 *
 * Any valid value is converted to an int8_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
int8_t nuts_value_to_i8(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to an uint64_t.
 *
 * Converts a napi_value containing a Number in an uint64_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a Number or BigInt not between 0 and C UINT64_MAX
 *
 * Any valid value is converted to an uint64_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
uint64_t nuts_value_to_u64(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to an uint32_t.
 *
 * Converts a napi_value containing a Number in an uint32_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a Number or BigInt not between 0 and C UINT32_MAX
 *
 * Any valid value is converted to an uint32_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
uint32_t nuts_value_to_u32(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to an uint16_t.
 *
 * Converts a napi_value containing a Number in an uint16_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a Number or BigInt not between 0 and C UINT16_MAX
 *
 * Any valid value is converted to an uint16_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
uint16_t nuts_value_to_u16(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to an uint8_t.
 *
 * Converts a napi_value containing a Number in an uint8_t.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number or BigInt.
 * - you pass a Number that is not integral.
 * - you pass a Number or BigInt not between 0 and C UINT8_MAX
 *
 * Any valid value is converted to an uint8_t and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
uint8_t nuts_value_to_u8(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to a `double`.
 *
 * Converts a napi_value containing a Number in a `double`.
 * Error argument is set in the following conditions:
 * - you pass a type different than Number.
 *
 * Any valid value is converted to a `double` and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
double nuts_value_to_double(napi_env env, napi_value value, char** error);

/**
 * @brief Converts a napi_value to a float.
 *
 * Converts a napi_value containing a Number in a `float`.
 * Error argument is set in the following conditions:
 * - you pass a type different from Number
 * - you pass a Number not between 0 and C UINT8_MAX
 *
 * Any valid value is converted to a `float` and returned to the
 * caller.
 *
 * The function returns 0 in case of error.
 *
 */
float nuts_value_to_float(napi_env env, napi_value value, char** error);


#define VAR_I64_FROM_JS(VAR_NAME, VALUE_JS) 											\
	int64_t VAR_NAME = nuts_value_to_i64(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR

#define VAR_U32_FROM_JS(VAR_NAME, VALUE_JS) 											\
	uint32_t VAR_NAME = nuts_value_to_u32(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR

#define VAR_I16_FROM_JS(VAR_NAME, VALUE_JS) 											\
	int16_t VAR_NAME = nuts_value_to_i16(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR

#define VAR_U16_FROM_JS(VAR_NAME, VALUE_JS) 											\
	uint16_t VAR_NAME = nuts_value_to_u16(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR

#define VAR_I8_FROM_JS(VAR_NAME, VALUE_JS) 												\
	int8_t VAR_NAME = nuts_value_to_i8(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR

#define VAR_U8_FROM_JS(VAR_NAME, VALUE_JS) 												\
	uint8_t VAR_NAME = nuts_value_to_u8(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR

#define VAR_DOUBLE_FROM_JS(VAR_NAME, VALUE_JS) 											\
	double VAR_NAME = nuts_value_to_double(env, VALUE_JS, &nuts_error); 				\
	_CHECK_NUTS_ERROR

#define VAR_FLOAT_FROM_JS(VAR_NAME, VALUE_JS) 											\
	float VAR_NAME = nuts_value_to_float(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR


#define VAR_U64_FROM_JS(VAR_NAME, VALUE_JS) 											\
	uint64_t VAR_NAME = nuts_value_to_u64(env, VALUE_JS, &nuts_error); 					\
	_CHECK_NUTS_ERROR


#define VAR_JS_FROM_I32(VAR_NAME, VALUE) 											\
	napi_value VAR_NAME = i32_to_value(env, VALUE, &nuts_error); 					\
	_CHECK_NUTS_ERROR


#define VAR_JS_FROM_STR(VAR_NAME, VALUE) 											\
	napi_value VAR_NAME = utf8_to_value(env, VALUE, &nuts_error); 					\
	_CHECK_NUTS_ERROR



#define I32(ARG_NAME) VAR_I32_FROM_JS(ARG_NAME, ((arg_idx < argc) ? (argv[arg_idx]) : nullptr))
#define STR(ARG_NAME) VAR_STR_FROM_JS(ARG_NAME, ((arg_idx < argc) ? (argv[arg_idx]) : nullptr))
#define VALUE(ARG_NAME) napi_value ARG_NAME = (arg_idx < argc) ? argv[arg_idx] : NULL;

#define NC_CALL(FN) {                                              \
    int retval = FN;                                               \
    if (retval != NC_NOERR) {                                      \
        napi_throw_error(env, NULL, nc_strerror(retval));          \
        return NULL;                                               \
    }                                                              \
}



#define RETURN_I32(VALUE)                                     \
	{                                                           \
		napi_value val = i32_to_value(env, VALUE, &nuts_error);   \
		if (nuts_error != NULL) {																  \
			napi_throw_error(env, NULL, nuts_error);							  \
			free(nuts_error);                                   	  \
			return NULL;															              \
		}																						              \
		return val; 																              \
	}


#define RETURN_STR(VALUE)                                                   \
	{                                                                         \
		napi_value val = utf8_to_value(env, VALUE, &nuts_error);           \
		if (nuts_error != NULL) {										                            \
			napi_throw_error(env, NULL, nuts_error);	                            \
			free(nuts_error);                                                     \
			return NULL;															                            \
		}																						                            \
		return val;                                                             \
	}



#define RETURN_I64(VALUE)                                                                           \
	{                                                                                               \
		napi_value val = nuts_i64_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                        \
	}


#define RETURN_U64(VALUE)                                                                          \
	{                                                                                              \
		napi_value val = nuts_u64_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                        \
	}

#define RETURN_U32(VALUE)                                                                          \
	{                                                                                              \
		napi_value val = nuts_u32_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                        \
	}

#define RETURN_U16(VALUE)                                                                          \
	{                                                                                              \
		napi_value val = nuts_u16_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                        \
	}

#define RETURN_I16(VALUE)                                                                          \
	{                                                                                              \
		napi_value val = nuts_i16_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                        \
	}


#define RETURN_U8(VALUE) \
	{                                                                                              \
		napi_value val = nuts_u8_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                        \
	}

#define RETURN_I8(VALUE) \
	{                                                                                              \
		napi_value val = nuts_i8_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                        \
	}


#define RETURN_DOUBLE(VALUE)                                                                      	\
	{                                                                                             	\
		napi_value val = nuts_double_to_value(env, VALUE, &nuts_error);                           	\
		if (nuts_error != NULL) {																	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                         		\
	}

#define RETURN_FLOAT(VALUE) 																		\
	{																			                    \
		napi_value val = nuts_double_to_value(env, VALUE, &nuts_error);                            	\
		if (nuts_error != NULL) {																 	\
			napi_throw_error(env, NULL, nuts_error);												\
			free(nuts_error);                                   									\
			return NULL;																			\
		}																							\
		return val;                                                                         		\
	}
