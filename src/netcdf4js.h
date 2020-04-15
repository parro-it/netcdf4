#ifndef NETCDF4JS_H
#define NETCDF4JS_H

#include <netcdf.h>
#include <node.h>
#include <string>

namespace netcdf4js {

inline void throw_netcdf_error(v8::Isolate* isolate, int retval) {
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, nc_strerror(retval), v8::NewStringType::kNormal).ToLocalChecked()));
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
