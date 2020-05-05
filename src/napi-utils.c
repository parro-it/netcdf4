#include "napi-utils.h"
#include <assert.h>
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * returns a string containing the name of given napi status
 */
const char *status_name(napi_status status) {
	switch (status) {
	case napi_ok:
		return "napi_ok";
	case napi_invalid_arg:
		return "napi_invalid_arg";
	case napi_object_expected:
		return "napi_object_expected";
	case napi_string_expected:
		return "napi_string_expected";
	case napi_name_expected:
		return "napi_name_expected";
	case napi_function_expected:
		return "napi_function_expected";
	case napi_number_expected:
		return "napi_number_expected";
	case napi_boolean_expected:
		return "napi_boolean_expected";
	case napi_array_expected:
		return "napi_array_expected";
	case napi_generic_failure:
		return "napi_generic_failure";
	case napi_pending_exception:
		return "napi_pending_exception";
	case napi_cancelled:
		return "napi_cancelled";
	case napi_escape_called_twice:
		return "napi_escape_called_twice";
	case napi_handle_scope_mismatch:
		return "napi_handle_scope_mismatch";
	case napi_callback_scope_mismatch:
		return "napi_callback_scope_mismatch";
	case napi_queue_full:
		return "napi_queue_full";
	case napi_closing:
		return "napi_closing";
	case napi_bigint_expected:
		return "napi_bigint_expected";
	case napi_date_expected:
		return "napi_date_expected";

		// not supported in 8 & 10
		/*case napi_arraybuffer_expected:
			return "napi_arraybuffer_expected";
		case napi_detachable_arraybuffer_expected:
			return "napi_detachable_arraybuffer_expected";*/
	}
	return NULL;
}

char *nuts_napi_err_f(napi_env env, const char *msg) {
	const napi_extended_error_info *err;
	napi_extended_error_info fatal_err;

	napi_status status = napi_get_last_error_info(env, &err);
	if (status != napi_ok) {
		fatal_err.error_message =
			"napi_get_last_error_info failed: cannot retrieve NAPI error information";
		fatal_err.error_code = status;
		err = &fatal_err;
	}

	if (err == NULL) {
		fatal_err.error_message = "no NAPI error occurred";
		fatal_err.error_code = napi_ok;
		err = &fatal_err;
	}

	int buffer_len = snprintf(NULL, 0, msg, err->error_message, status_name(err->error_code));
	char *err_str = malloc(buffer_len + 1);
	snprintf(err_str, buffer_len + 1, msg, err->error_message, status_name(err->error_code));
	return err_str;
}

static char *invalid_value_err(napi_env env, napi_value value, char *err) {
	const char *frmt = "Invalid value `%s`: %s";
	char *nuts_error = NULL;
	char *value_s_c = value_to_utf8(env, value, &nuts_error);
	int error_len = snprintf(NULL, 0, frmt, value_s_c, err);
	char *error = malloc(error_len + 1);
	snprintf(error, error_len + 1, frmt, value_s_c, err);
	free(err);
	free(value_s_c);
	return error;
}

static int32_t value_to_int(int32_t min, int32_t max, napi_env env, napi_value value,
							char **error) {
	*error = NULL;
	int32_t c_value;

	napi_status status = napi_get_value_int32(env, value, &c_value);
	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return 0;
	}

	return c_value;
}

int32_t value_to_i32(napi_env env, napi_value value, char **error) {
	return (int32_t)value_to_int(INT32_MIN, INT32_MAX, env, value, error);
}

int64_t nuts_value_to_i64(napi_env env, napi_value value, char **error) {
	*error = NULL;
	int64_t c_value;

	napi_valuetype value_type;
	napi_status status = napi_typeof(env, value, &value_type);
	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return 0;
	}

	if (value_type == napi_bigint) {
		bool lossless;
		status = napi_get_value_bigint_int64(env, value, &c_value, &lossless);
		if (!lossless || status != napi_ok) {
			*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
			return 0;
		}
	} else {
		status = napi_get_value_int64(env, value, &c_value);
		if (status != napi_ok) {
			*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
			return 0;
		}
	}

	return c_value;
}

int16_t nuts_value_to_i16(napi_env env, napi_value value, char **error) {
	return (int16_t)value_to_int(INT16_MIN, INT16_MAX, env, value, error);
}

int8_t nuts_value_to_i8(napi_env env, napi_value value, char **error) {
	return (int8_t)value_to_int(INT8_MIN, INT8_MAX, env, value, error);
}

static uint32_t nuts_value_to_uint(uint32_t max, napi_env env, napi_value value, char **error) {
	*error = NULL;
	uint32_t c_value;

	napi_status status = napi_get_value_uint32(env, value, &c_value);
	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return 0;
	}

	return c_value;
}

uint32_t nuts_value_to_u32(napi_env env, napi_value value, char **error) {
	return nuts_value_to_uint(UINT32_MAX, env, value, error);
}

uint64_t nuts_value_to_u64(napi_env env, napi_value value, char **error) {
	*error = NULL;
	uint64_t c_value;

	bool lossless;
	napi_status status = napi_get_value_bigint_uint64(env, value, &c_value, &lossless);
	if (!lossless || status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return 0;
	}

	return c_value;
}

uint16_t nuts_value_to_u16(napi_env env, napi_value value, char **error) {
	return (uint16_t)nuts_value_to_uint(UINT16_MAX, env, value, error);
}

uint8_t nuts_value_to_u8(napi_env env, napi_value value, char **error) {
	return (uint8_t)nuts_value_to_uint(UINT8_MAX, env, value, error);
}

double nuts_value_to_double(napi_env env, napi_value value, char **error) {
	double c_value;
	napi_status status = napi_get_value_double(env, value, &c_value);

	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return 0;
	}

	return c_value;
}

float nuts_value_to_float(napi_env env, napi_value value, char **error) {
	*error = NULL;
	double c_value = nuts_value_to_double(env, value, error);

	return (float)c_value;
}

napi_value nuts_i64_to_value(napi_env env, int64_t c_value, char **error) {
	napi_value value;
	napi_status status = napi_create_bigint_int64(env, c_value, &value);
	if (status != napi_ok) {
		INVALID_C_VALUE_ERR(*error, "%ld", c_value, nuts_napi_err_f(env, "%s (%s)"));
		return NULL;
	}
	return value;
}

napi_value nuts_i32_to_value(napi_env env, int32_t c_value, char **error) {
	napi_value value;
	napi_status status = napi_create_int32(env, c_value, &value);
	if (status != napi_ok) {
		INVALID_C_VALUE_ERR(*error, "%d", c_value, nuts_napi_err_f(env, "%s (%s)"));
		return NULL;
	}
	return value;
}

napi_value nuts_i16_to_value(napi_env env, int16_t c_value, char **error) {
	return nuts_i32_to_value(env, (int32_t)c_value, error);
}

napi_value nuts_i8_to_value(napi_env env, int8_t c_value, char **error) {
	return nuts_i32_to_value(env, (int32_t)c_value, error);
}

napi_value nuts_u64_to_value(napi_env env, uint64_t c_value, char **error) {
	napi_value value;
	napi_status status = napi_create_bigint_uint64(env, c_value, &value);
	if (status != napi_ok) {
		INVALID_C_VALUE_ERR(*error, "%lu", c_value, nuts_napi_err_f(env, "%s (%s)"));
		return NULL;
	}
	return value;
}

napi_value nuts_u32_to_value(napi_env env, uint32_t c_value, char **error) {
	napi_value value;
	napi_status status = napi_create_uint32(env, c_value, &value);
	if (status != napi_ok) {
		INVALID_C_VALUE_ERR(*error, "%u", c_value, nuts_napi_err_f(env, "%s (%s)"));
		return NULL;
	}
	return value;
}

napi_value nuts_u16_to_value(napi_env env, uint16_t c_value, char **error) {
	return nuts_u32_to_value(env, (uint32_t)c_value, error);
}

napi_value nuts_u8_to_value(napi_env env, uint8_t c_value, char **error) {
	return nuts_u32_to_value(env, (uint32_t)c_value, error);
}

napi_value nuts_double_to_value(napi_env env, double c_value, char **error) {
	napi_value value_js;
	napi_status status = napi_create_double(env, c_value, &value_js);
	if (status != napi_ok) {
		INVALID_C_VALUE_ERR(*error, "%f", c_value, nuts_napi_err_f(env, "%s (%s)"));
		return NULL;
	}
	return value_js;
}

napi_value nuts_float_to_value(napi_env env, float c_value, char **error) {
	return nuts_double_to_value(env, (double)c_value, error);
}

char *value_to_utf8(napi_env env, napi_value value, char **error) {

	napi_valuetype value_type;
	napi_status status = napi_typeof(env, value, &value_type);
	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return NULL;
	}

	if (value_type == napi_null) {
		return NULL;
	}

	char *c_value = NULL;

	size_t len = 0;

	status = napi_get_value_string_utf8(env, value, NULL, 0, &len);
	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return NULL;
	}

	c_value = malloc(len + 1);

	status = napi_get_value_string_utf8(env, value, c_value, len + 1, &len);
	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		free(c_value);
		return NULL;
	}

	return c_value;
}

napi_value i32_to_value(napi_env env, int32_t c_value, char **error) {
	napi_value value;
	napi_status status = napi_create_int32(env, c_value, &value);
	if (status != napi_ok) {
		napi_throw_type_error(env, NULL, "invalid C value for i32");
		return NULL;
	}
	return value;
}

napi_value utf8_to_value(napi_env env, char *c_value, char **error) {
	if (c_value == NULL) {
		napi_value null;
		napi_status status = napi_get_null(env, &null);
		if (status != napi_ok) {
			napi_throw_type_error(env, NULL, "invalid C value for string");
			return NULL;
		}
		return null;
	}

	napi_value value_js;
	napi_status status = napi_create_string_utf8(env, c_value, NAPI_AUTO_LENGTH, &value_js);
	if (status != napi_ok) {
		napi_throw_type_error(env, NULL, "invalid C value for string");
		return NULL;
	}
	return value_js;
}

// return a js error based on the last napi error occurred.
napi_value create_napi_error(napi_env env, const napi_extended_error_info *napi_err) {
	char *nuts_error = NULL;
	napi_status status;

	napi_value err_code = i32_to_value(env, napi_err->error_code, &nuts_error);
	assert(nuts_error == NULL);
	napi_value err_msg = utf8_to_value(env, (char *)napi_err->error_message, &nuts_error);
	assert(nuts_error == NULL);

	napi_value error;
	status = napi_create_error(env, err_code, err_msg, &error);
	if (status != napi_ok) {
		printf("Cannot create error: orig code: %d, original napi error: %s\n",
			   napi_err->error_code, napi_err->error_message);
	}
	assert(status == napi_ok);

	return error;
}

// return NULL if status_to_check is napi_ok, or the exception
// occurred if there is a pending one, or an error describing
// then napi error occurred.
napi_value error_for_napi_status(napi_env env, napi_status status_to_check) {
	if (status_to_check == napi_ok) {
		// no error nor exception pending, return NULL
		return NULL;
	}
	napi_status status;

	// retrieve additional err info
	const napi_extended_error_info *tmp = NULL;
	status = napi_get_last_error_info(env, &tmp);
	assert(status == napi_ok);
	assert(tmp != NULL);

	// make a copy of the retrieved values, because
	// the content are valid only until next napi call.
	napi_extended_error_info napi_err;
	if (tmp->error_message != NULL) {
		napi_err.error_message = malloc(1 + strlen(tmp->error_message));
		strcpy((char *)napi_err.error_message, tmp->error_message);
	} else {
		napi_err.error_message = NULL;
	}
	napi_err.error_code = tmp->error_code;

	bool is_pending = status_to_check == napi_pending_exception;

	// if status is different from pending exception,
	// it contain the status of the error occurred, but we
	// have to check because an exception could be pending anyway.
	// in this case, we discard the errorr status
	// and return instead the exception
	if (!is_pending) {
		status = napi_is_exception_pending(env, &is_pending);
		assert(status == napi_ok);
	}

	if (is_pending) {
		// an exception is pending. return it
		// to the caller.
		napi_value err_js;
		status = napi_get_and_clear_last_exception(env, &err_js);
		assert(status == napi_ok);
		return err_js;
	}

	// return an exception base on the
	// napi error description.
	return create_napi_error(env, &napi_err);
}

int32_t nuts_value_to_i32(napi_env env, napi_value value, char **error) {
	return (int32_t)value_to_int(INT32_MIN, INT32_MAX, env, value, error);
}

bool nuts_value_to_bool(napi_env env, napi_value value, char **error) {
	bool c_value;
	napi_status status = napi_get_value_bool(env, value, &c_value);
	if (status != napi_ok) {
		*error = invalid_value_err(env, value, nuts_napi_err_f(env, "%s (%s)"));
		return false;
	}
	return c_value;
}
