#ifndef NETCDF4JS_VARIABLE_H
#define NETCDF4JS_VARIABLE_H

#include <netcdf.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string>

namespace netcdf4js {

    class Variable : public node::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
        Variable(const int& id_, const int& parent_id_);
        const std::string& get_name() const {
            return name;
        };

    private:
        static void ReadSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
        // TODO read whole data
        static v8::Persistent<v8::Function> constructor;
        static const unsigned char type_sizes[];
        int id;
        int parent_id;
        nc_type type;
        std::string name;
        // TODO attributes
        // TODO dimensions
    };
}

#endif
