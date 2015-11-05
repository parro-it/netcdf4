#ifndef NETCDF4JS_FILE_H
#define NETCDF4JS_FILE_H

#include <node.h>
#include <node_object_wrap.h>
#include <memory>

namespace netcdf4js {
    class Group;

    class File : public node::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);

    private:
        enum FileMode {
            READ
        };
        explicit File(const char* filename, const FileMode& mode);
        ~File();

        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static v8::Persistent<v8::Function> constructor;

        std::unique_ptr<Group> root;
        int id;
    };
}

#endif
