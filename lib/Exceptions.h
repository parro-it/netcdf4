#ifndef NETCDF4JS_EXCEPTIONS_H
#define NETCDF4JS_EXCEPTIONS_H

#include <node.h>

namespace netcdf4js {
    const char* get_error_message(int retval);

#define call_netcdf(retval) if (const char* s = get_error_message(retval)) { \
        v8::Isolate::GetCurrent()->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), s))); \
        return; \
    }

}

#endif
