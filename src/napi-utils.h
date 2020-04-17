#define NAPI_EXPERIMENTAL 1
#include <node_api.h>

#define SUPPRESS_UNUSED_WARNING(x) (void)(x)

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

  napi_value i32_to_value(napi_env env, int32_t c_value, char **error);
  int32_t value_to_i32(napi_env env, napi_value value, char **error);
  napi_value utf8_to_value(napi_env env, char *c_value, char **error);
  char *value_to_utf8(napi_env env, napi_value value, char **error);

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
	if (argc < (N - OPT_N) || argc > N) {                                               \
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


#define VAR_JS_FROM_I32(VAR_NAME, VALUE) 											\
	napi_value VAR_NAME = i32_to_value(env, VALUE, &nuts_error); 					\
	_CHECK_NUTS_ERROR


#define VAR_JS_FROM_STR(VAR_NAME, VALUE) 											\
	napi_value VAR_NAME = utf8_to_value(env, VALUE, &nuts_error); 					\
	_CHECK_NUTS_ERROR

#define I32(ARG_NAME) VAR_I32_FROM_JS(ARG_NAME, ((arg_idx < argc) ? (argv[arg_idx]) : nullptr))
#define STR(ARG_NAME) VAR_STR_FROM_JS(ARG_NAME, ((arg_idx < argc) ? (argv[arg_idx]) : nullptr))

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
