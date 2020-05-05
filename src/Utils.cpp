#include "Utils.h"
#include <netcdf.h>
#include <node_api.h>

namespace netcdf4js {

const unsigned char type_sizes[] = {
	0, // NC_NAT // unknown type
	1, // NC_BYTE
	1, // NC_CHAR
	2, // NC_SHORT
	4, // NC_INT / NC_LONG
	4, // NC_FLOAT
	8, // NC_DOUBLE
	1, // NC_UBYTE
	2, // NC_USHORT
	4  // NC_UINT
};

napi_typedarray_type typedarray_cons_per_type(napi_env env, int type) {
	switch (type) {
	case NC_BYTE:
	case NC_CHAR:
		return napi_int8_array;
	case NC_SHORT:
		return napi_int16_array;
	case NC_INT:
		return napi_int32_array;
	case NC_FLOAT:
		return napi_float32_array;
	case NC_DOUBLE:
		return napi_float64_array;
	case NC_UBYTE:
		return napi_uint8_array;
	case NC_USHORT:
		return napi_uint16_array;
	case NC_UINT:
		return napi_uint32_array;
	default:
		napi_throw_error(env, NULL, "Variable type not supported yet");
		return (napi_typedarray_type)-1;
	}
}
} // namespace netcdf4js
