#include "Variable.h"
#include "Exceptions.h"
#include "Dimension.h"
#include "Attribute.h"
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
        8, // NC_INT64 // no TypedArray for this type
        8 // NC_UINT64 // no TypedArray for this type
    };

    v8::Persistent<v8::Function> Variable::constructor;

    Variable::Variable(const int& id_, const int& parent_id_)
        : id(id_), parent_id(parent_id_) {
        call_netcdf(nc_inq_vartype(parent_id, id, &type));
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance();
        Wrap(obj);
    }

    void Variable::Init(v8::Local<v8::Object> exports) {
        v8::Isolate* isolate = exports->GetIsolate();

        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
        tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Variable"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "readSlice", ReadSlice);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "dimensions"), Variable::GetDimensions);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "attributes"), Variable::GetAttributes);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name"), Variable::GetName, Variable::SetName);

        constructor.Reset(isolate, tpl->GetFunction());
    }

    bool Variable::get_name(char* name) const {
        call_netcdf_bool(nc_inq_varname(parent_id, id, name));
        return true;
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

    void Variable::GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Isolate* isolate = info.GetIsolate();
        Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
        int ndims;
        call_netcdf(nc_inq_varndims(obj->parent_id, obj->id, &ndims));
        std::vector<int> dim_ids(ndims);
        call_netcdf(nc_inq_vardimid(obj->parent_id, obj->id, &dim_ids[0]));
        v8::Local<v8::Object> result = v8::Object::New(isolate);
        char name[NC_MAX_NAME+1];
        for (int dim_id : dim_ids) {
           Dimension* d = new Dimension(dim_id, obj->parent_id);
           if (d->get_name(name)) {
               result->Set(v8::String::NewFromUtf8(isolate, name), d->handle());
           } else {
               return;
           }
        }
        info.GetReturnValue().Set(result);
    }

    void Variable::GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Isolate* isolate = info.GetIsolate();
        Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
        int natts;
        call_netcdf(nc_inq_varnatts(obj->parent_id, obj->id, &natts));
        v8::Local<v8::Object> result = v8::Object::New(isolate);
        char name[NC_MAX_NAME+1];
        for (int i = 0; i < natts; i++) {
            call_netcdf(nc_inq_attname(obj->id, obj->id, i, name));
            Attribute* a = new Attribute(name, obj->id, obj->parent_id);
            result->Set(v8::String::NewFromUtf8(isolate, name), a->handle());
        }
        info.GetReturnValue().Set(result);
    }

    void Variable::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
        v8::Isolate* isolate = info.GetIsolate();
        Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
        char name[NC_MAX_NAME+1];
        if (obj->get_name(name)) {
            info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name));
        }
    }

    void Variable::SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
        Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
        v8::String::Utf8Value new_name_(val->ToString());
        call_netcdf(nc_rename_var(obj->parent_id, obj->id, *new_name_));
    }
}
