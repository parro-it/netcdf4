#ifndef NETCDF4JS_FILE_H
#define NETCDF4JS_FILE_H

#include <node.h>
#include <node_object_wrap.h>

namespace netcdf4js {
    class Group;

    class File : public node::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);

    private:
        explicit File(const int& id_);
        ~File();

        bool open(const char* filename, const int& mode, const int& format);
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Sync(const v8::FunctionCallbackInfo<v8::Value>& args);
        static v8::Persistent<v8::Function> constructor;

        int id;
        bool closed;
    };
}

#endif
