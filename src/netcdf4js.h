#ifndef NETCDF4JS_H
#define NETCDF4JS_H

#include <netcdf.h>
#include <node.h>
#include <string>

namespace netcdf4js {

#define call_netcdf(retval)                                                                                                                      \
    if (int r = retval != NC_NOERR) {                                                                                                            \
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nc_strerror(r)))); \
        return;                                                                                                                                  \
    }

#define call_netcdf_bool(retval)                                                                                                                 \
    if (int r = retval != NC_NOERR) {                                                                                                            \
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nc_strerror(r)))); \
        return false;                                                                                                                            \
    }

inline int get_type(const std::string& type_str) {
    if (type_str == "byte") {
        return NC_BYTE;
    } else if (type_str == "char") {
        return NC_CHAR;
    } else if (type_str == "short") {
        return NC_SHORT;
    } else if (type_str == "int") {
        return NC_INT;
    } else if (type_str == "float") {
        return NC_FLOAT;
    } else if (type_str == "double") {
        return NC_DOUBLE;
    } else if (type_str == "ubyte") {
        return NC_UBYTE;
    } else if (type_str == "ushort") {
        return NC_USHORT;
    } else if (type_str == "uint") {
        return NC_UINT;
    } else if (type_str == "string") {
        return NC_STRING;
    } else {
        return NC2_ERR;
    }
}
}  // namespace netcdf4js

#endif
