#include "File.h"
#include <netcdf.h>
#include <string>
#include "Group.h"
#include "Variable.h"
#include "netcdf4js.h"

namespace netcdf4js {

v8::Persistent<v8::Function> File::constructor;

File::File(const int& id_) : id(id_), closed(false) {}

File::~File() {
    if (!closed) {
        call_netcdf(nc_close(id));
    }
}

void File::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "File"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "sync", File::Sync);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", File::Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", File::Inspect);
    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "File"), tpl->GetFunction());
}

void File::New(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (args.Length() < 2) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    if (args.IsConstructCall()) {
        std::string filename = *v8::String::Utf8Value(isolate, args[0]->ToString());
        std::string mode_arg = *v8::String::Utf8Value(isolate, args[1]->ToString());
        int format = NC_NETCDF4;
        int id;
        if (args.Length() > 2) {
            std::string format_arg = *v8::String::Utf8Value(isolate, args[2]->ToString());
            if (format_arg == "classic") {
                format = 0;
            } else if (format_arg == "classic64") {
                format = NC_64BIT_OFFSET;
            } else if (format_arg == "netcdf4") {
                format = NC_NETCDF4;
            } else if (format_arg == "netcdf4classic") {
                format = NC_NETCDF4 | NC_CLASSIC_MODEL;
            } else {
                isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown file format")));
                return;
            }
        }
        if (mode_arg == "r") {
            call_netcdf(nc_open(filename.c_str(), NC_NOWRITE, &id));
        } else if (mode_arg == "w") {
            call_netcdf(nc_open(filename.c_str(), NC_WRITE, &id));
        } else if (mode_arg == "c") {
            call_netcdf(nc_create(filename.c_str(), format | NC_NOCLOBBER, &id));
        } else if (mode_arg == "c!") {
            call_netcdf(nc_create(filename.c_str(), format | NC_CLOBBER, &id));
        } else {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown file mode")));
            return;
        }
        File* obj = new File(id);
        obj->Wrap(args.This());
        args.This()->Set(v8::String::NewFromUtf8(isolate, "root"), (new Group(id))->handle());
        args.GetReturnValue().Set(args.This());
    } else {
        const int argc = 1;
        v8::Local<v8::Value> argv[argc] = {args[0]};
        v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(isolate->GetCurrentContext(), argc, argv).ToLocalChecked());
    }
}

void File::Sync(const v8::FunctionCallbackInfo<v8::Value>& args) {
    File* obj = node::ObjectWrap::Unwrap<File>(args.Holder());
    call_netcdf(nc_sync(obj->id));
}

void File::Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
    File* obj = node::ObjectWrap::Unwrap<File>(args.Holder());
    call_netcdf(nc_close(obj->id));
    obj->closed = true;
}

void File::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object File]"));
}
}  // namespace netcdf4js
