#include "Variable.h"
#include "Exceptions.h"
#include <vector>

namespace netcdf4js {

    const unsigned char Variable::type_sizes[] = {
        0, // NC_NAT // unknown type
        1, // NC_BYTE
        1, // NC_CHAR
        2, // NC_SHORT
        4, // NC_INT / NC_LONG
        4, // NC_FLOAT
        8, // NC_DOUBLE
        1, // NC_UBYTE
        2, // NC_USHORT
        4, // NC_UINT
        8, // NC_INT64 // js has no int64
        8 // NC_UINT64 // js has no int64
    };
    v8::Persistent<v8::Function> Variable::constructor;

    Variable::Variable(const int& id_, const int& parent_id_)
        : id(id_), parent_id(parent_id_) {
        char name_[NC_MAX_NAME+1];
        int ndims;
        int nattrs;
        call_netcdf(nc_inq_var(parent_id, id, name_, &type, &ndims, nullptr, &nattrs));
        name = std::string(name_);
        // TODO
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance();
        obj->Set(v8::String::NewFromUtf8(isolate, "name"), v8::String::NewFromUtf8(isolate, name.c_str()));
        Wrap(obj);
    }

    void Variable::Init(v8::Local<v8::Object> exports) {
        v8::Isolate* isolate = exports->GetIsolate();

        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
        tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Variable"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "readSlice", ReadSlice);

        constructor.Reset(isolate, tpl->GetFunction());
    }

    void Variable::ReadSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();

        if (args.Length() % 2 != 0) {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
            return;
        }

        Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());

        if (obj->type < NC_BYTE || obj->type > NC_UINT) {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
        }

        std::vector<size_t> pos(args.Length() / 2);
        std::vector<size_t> size(args.Length() / 2);
        size_t total_size = 1;
        for (int i = 0; i < args.Length() / 2; i++) {
            pos[i] = args[2*i]->IntegerValue();
            size_t s = args[2*i+1]->IntegerValue();
            size[i] = s;

            total_size *= s;
        }
        v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, total_size * type_sizes[obj->type]);
        call_netcdf(nc_get_vara(obj->parent_id, obj->id, &pos[0], &size[0], buffer->GetContents().Data()));
        v8::Local<v8::Object> result;

        switch (obj->type) {
        case NC_BYTE:
            result = v8::Int8Array::New(buffer, 0, total_size);
            break;
        case NC_CHAR:
            result = v8::Int8Array::New(buffer, 0, total_size);
            break;
        case NC_SHORT:
            result = v8::Int16Array::New(buffer, 0, total_size);
            break;
        case NC_INT:
            result = v8::Int32Array::New(buffer, 0, total_size);
            break;
        case NC_FLOAT:
            result = v8::Float32Array::New(buffer, 0, total_size);
            break;
        case NC_DOUBLE:
            result = v8::Float64Array::New(buffer, 0, total_size);
            break;
        case NC_UBYTE:
            result = v8::Uint8Array::New(buffer, 0, total_size);
            break;
        case NC_USHORT:
            result = v8::Uint16Array::New(buffer, 0, total_size);
            break;
        case NC_UINT:
            result = v8::Uint32Array::New(buffer, 0, total_size);
            break;
        }
        args.GetReturnValue().Set(result);
    }
}
