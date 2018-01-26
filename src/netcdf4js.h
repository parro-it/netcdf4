#ifndef NETCDF4JS_H
#define NETCDF4JS_H

#include <node.h>

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

}

#endif
