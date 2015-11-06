#include "File.h"
#include "Group.h"
#include "Exceptions.h"
#include "Variable.h"
#include <netcdf.h>
#include <string>

namespace netcdf4js {
    v8::Persistent<v8::Function> File::constructor;

    File::File() {
        id = -1;
    }

    File::~File() {
        if (id >= 0) {
            call_netcdf(nc_close(id));
        }
    }

    bool File::open(const char* filename, const FileMode& mode) {
        switch (mode) {
        case WRITE:
            call_netcdf_bool(nc_open(filename, NC_WRITE, &id));
            break;
        case READ:
            call_netcdf_bool(nc_open(filename, NC_NOWRITE, &id));
            break;
        case CREATE:
            call_netcdf_bool(nc_create(filename, NC_NETCDF4 | NC_NOCLOBBER, &id));
            break;
        case REPLACE:
            call_netcdf_bool(nc_create(filename, NC_NETCDF4 | NC_CLOBBER, &id));
            break;
        }
        return true;
    }

    void File::Init(v8::Local<v8::Object> exports) {
        v8::Isolate* isolate = exports->GetIsolate();

        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
        tpl->SetClassName(v8::String::NewFromUtf8(isolate, "File"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

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
            FileMode mode;
            std::string mode_arg = *v8::String::Utf8Value(args[1]->ToString());
            if (mode_arg == "r") {
                mode = READ;
            } else if (mode_arg == "w") {
                mode = WRITE;
            } else if (mode_arg == "c") {
                mode = CREATE;
            } else if (mode_arg == "c!") {
                mode = REPLACE;
            } else {
                isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown file mode")));
                return;
            }
            File* obj = new File();
            if (!obj->open(*v8::String::Utf8Value(args[0]->ToString()), mode)) {
                delete obj;
            } else {
                obj->Wrap(args.This());
                args.This()->Set(v8::String::NewFromUtf8(isolate, "root"), (new Group(obj->id))->handle());
                args.GetReturnValue().Set(args.This());
            }
        } else {
            const int argc = 1;
            v8::Local<v8::Value> argv[argc] = { args[0] };
            v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
            args.GetReturnValue().Set(cons->NewInstance(argc, argv));
        }
    }
}
