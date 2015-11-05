#include "File.h"
#include "Group.h"
#include "Exceptions.h"
#include "Variable.h"
#include <netcdf.h>

namespace netcdf4js {
    v8::Persistent<v8::Function> File::constructor;

    File::File(const char* filename, const FileMode& mode) {
        switch (mode) {
            //case WRITE:
            //ncCheck(nc_open(filename, NC_WRITE, &id), isolate);
            //break;
        case READ:
            call_netcdf(nc_open(filename, NC_NOWRITE, &id));
            root.reset(new Group(id));
            break;
            //case CREATE:
            //ncCheck(nc_create(filename, NC_NETCDF4 | NC_NOCLOBBER, &id), isolate);
            //break;
            //case REPLACE:
            //ncCheck(nc_create(filename, NC_NETCDF4 | NC_CLOBBER, &id), isolate);
            //break;
        }
    }

    File::~File() {
        call_netcdf(nc_close(id));
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
            v8::String::Utf8Value filename(args[0]->ToString());
            FileMode mode = READ; // TODO support other modes
            File* obj = new File(*filename, mode);
            obj->Wrap(args.This());
            args.This()->Set(v8::String::NewFromUtf8(isolate, "root"), obj->root->handle());
            args.GetReturnValue().Set(args.This());
        } else {
            const int argc = 1;
            v8::Local<v8::Value> argv[argc] = { args[0] };
            v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
            args.GetReturnValue().Set(cons->NewInstance(argc, argv));
        }
    }
}
