#include "netcdf4js.h"
#include "Variable.h"
#include "Dimension.h"
#include "Attribute.h"

namespace netcdf4js {

const unsigned char Variable::type_sizes[] = {
    0,  // NC_NAT // unknown type
    1,  // NC_BYTE
    1,  // NC_CHAR
    2,  // NC_SHORT
    4,  // NC_INT / NC_LONG
    4,  // NC_FLOAT
    8,  // NC_DOUBLE
    1,  // NC_UBYTE
    2,  // NC_USHORT
    4   // NC_UINT
};

const char* Variable::type_names[] = {
    "unknown",  // NC_NAT // unknown type
    "byte",     // NC_BYTE
    "char",     // NC_CHAR
    "short",    // NC_SHORT
    "int",      // NC_INT / NC_LONG
    "float",    // NC_FLOAT
    "double",   // NC_DOUBLE
    "ubyte",    // NC_UBYTE
    "ushort",   // NC_USHORT
    "uint"      // NC_UINT
};

v8::Persistent<v8::Function> Variable::constructor;

Variable::Variable(const int& id_, const int& parent_id_) : id(id_), parent_id(parent_id_) {
    call_netcdf(nc_inq_var(parent_id, id, NULL, &type, &ndims, NULL, NULL));
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance();
    Wrap(obj);
}

void Variable::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();

    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Variable"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "read", Variable::Read);
    NODE_SET_PROTOTYPE_METHOD(tpl, "readSlice", Variable::ReadSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "readStridedSlice", Variable::ReadStridedSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "write", Variable::Write);
    NODE_SET_PROTOTYPE_METHOD(tpl, "writeSlice", Variable::WriteSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "writeStridedSlice", Variable::WriteStridedSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addAttribute", Variable::AddAttribute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Variable::Inspect);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "id"), Variable::GetId);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "type"), Variable::GetType);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "dimensions"), Variable::GetDimensions);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "attributes"), Variable::GetAttributes);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name"), Variable::GetName, Variable::SetName);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "endianness"), Variable::GetEndianness, Variable::SetEndianness);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "checksummode"), Variable::GetChecksumMode, Variable::SetChecksumMode);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "chunkmode"), Variable::GetChunkMode, Variable::SetChunkMode);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "chunksizes"), Variable::GetChunkSizes, Variable::SetChunkSizes);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "fillmode"), Variable::GetFillMode, Variable::SetFillMode);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "fillvalue"), Variable::GetFillValue, Variable::SetFillValue);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "compressionshuffle"), Variable::GetCompressionShuffle,
                                         Variable::SetCompressionShuffle);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "compressiondeflate"), Variable::GetCompressionDeflate,
                                         Variable::SetCompressionDeflate);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "compressionlevel"), Variable::GetCompressionLevel, Variable::SetCompressionLevel);
    constructor.Reset(isolate, tpl->GetFunction());
}

bool Variable::get_name(char* name) const {
    call_netcdf_bool(nc_inq_varname(parent_id, id, name));
    return true;
}

void Variable::Write(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    size_t pos[obj->ndims];
    size_t size[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = args[i]->IntegerValue();
        size[i] = 1;
    }
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR: {
            int8_t v = args[obj->ndims]->Int32Value();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        case NC_SHORT: {
            int16_t v = args[obj->ndims]->Int32Value();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        case NC_INT: {
            int32_t v = args[obj->ndims]->Int32Value();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        case NC_FLOAT: {
            float v = args[obj->ndims]->NumberValue();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        case NC_DOUBLE: {
            double v = args[obj->ndims]->NumberValue();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        case NC_UBYTE: {
            uint8_t v = args[obj->ndims]->Uint32Value();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        case NC_USHORT: {
            uint16_t v = args[obj->ndims]->Uint32Value();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        case NC_UINT: {
            uint32_t v = args[obj->ndims]->Uint32Value();
            call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, &v));
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
}

void Variable::WriteSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != 2 * obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    if (!args[2 * obj->ndims]->IsTypedArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a typed array")));
        return;
    }
    size_t pos[obj->ndims];
    size_t size[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = args[2 * i]->IntegerValue();
        size_t s = args[2 * i + 1]->IntegerValue();
        size[i] = s;
        total_size *= s;
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[2 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array")));
        return;
    }

    bool correct_type;
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR:
            correct_type = val->IsInt8Array();
            break;
        case NC_SHORT:
            correct_type = val->IsInt16Array();
            break;
        case NC_INT:
            correct_type = val->IsInt32Array();
            break;
        case NC_FLOAT:
            correct_type = val->IsFloat32Array();
            break;
        case NC_DOUBLE:
            correct_type = val->IsFloat64Array();
            break;
        case NC_UBYTE:
            correct_type = val->IsUint8Array();
            break;
        case NC_USHORT:
            correct_type = val->IsUint16Array();
            break;
        case NC_UINT:
            correct_type = val->IsUint32Array();
            break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type")));
        return;
    }
    call_netcdf(nc_put_vara(obj->parent_id, obj->id, pos, size, val->Buffer()->GetContents().Data()));
}

void Variable::WriteStridedSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != 3 * obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    if (!args[3 * obj->ndims]->IsTypedArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a typed array")));
        return;
    }
    size_t pos[obj->ndims];
    size_t size[obj->ndims];
    ptrdiff_t stride[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = args[3 * i]->IntegerValue();
        size_t s = args[3 * i + 1]->IntegerValue();
        size[i] = s;
        total_size *= s;
        stride[i] = args[3 * i + 2]->IntegerValue();
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[3 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array")));
        return;
    }

    bool correct_type;
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR:
            correct_type = val->IsInt8Array();
            break;
        case NC_SHORT:
            correct_type = val->IsInt16Array();
            break;
        case NC_INT:
            correct_type = val->IsInt32Array();
            break;
        case NC_FLOAT:
            correct_type = val->IsFloat32Array();
            break;
        case NC_DOUBLE:
            correct_type = val->IsFloat64Array();
            break;
        case NC_UBYTE:
            correct_type = val->IsUint8Array();
            break;
        case NC_USHORT:
            correct_type = val->IsUint16Array();
            break;
        case NC_UINT:
            correct_type = val->IsUint32Array();
            break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type")));
        return;
    }
    call_netcdf(nc_put_vars(obj->parent_id, obj->id, pos, size, stride, val->Buffer()->GetContents().Data()));
}

void Variable::Read(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
        return;
    }
    size_t pos[obj->ndims];
    size_t size[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = args[i]->IntegerValue();
        size[i] = 1;
    }
    v8::Local<v8::Value> result;
    switch (obj->type) {
        case NC_BYTE: {
            int8_t v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_CHAR: {
            char v[2];
            v[1] = 0;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::String::NewFromUtf8(isolate, v);
        } break;
        case NC_SHORT: {
            int16_t v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_INT: {
            int32_t v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_FLOAT: {
            float v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Number::New(isolate, v);
        } break;
        case NC_DOUBLE: {
            double v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Number::New(isolate, v);
        } break;
        case NC_UBYTE: {
            uint8_t v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_USHORT: {
            uint16_t v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_UINT: {
            uint32_t v;
            call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
    args.GetReturnValue().Set(result);
}

void Variable::ReadSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != 2 * obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
        return;
    }

    size_t pos[obj->ndims];
    size_t size[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = args[2 * i]->IntegerValue();
        size_t s = args[2 * i + 1]->IntegerValue();
        size[i] = s;
        total_size *= s;
    }
    v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, total_size * type_sizes[obj->type]);
    call_netcdf(nc_get_vara(obj->parent_id, obj->id, pos, size, buffer->GetContents().Data()));
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

void Variable::ReadStridedSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != 3 * obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
        return;
    }

    size_t pos[obj->ndims];
    size_t size[obj->ndims];
    ptrdiff_t stride[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = args[3 * i]->IntegerValue();
        size_t s = args[3 * i + 1]->IntegerValue();
        size[i] = s;
        total_size *= s;
        stride[i] = args[3 * i + 2]->IntegerValue();
    }
    v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, total_size * type_sizes[obj->type]);
    call_netcdf(nc_get_vars(obj->parent_id, obj->id, pos, size, stride, buffer->GetContents().Data()));
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

void Variable::AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() < 2) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    Attribute* res = new Attribute(*v8::String::Utf8Value(args[0]), obj->id, obj->parent_id);
    res->set_value(args[1]);
    args.GetReturnValue().Set(res->handle());
}

void Variable::GetId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
}

void Variable::GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int dim_ids[obj->ndims];
    call_netcdf(nc_inq_vardimid(obj->parent_id, obj->id, dim_ids));
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    for (int i = 0; i < obj->ndims; i++) {
        Dimension* d = new Dimension(dim_ids[i], obj->parent_id);
        result->Set(i, d->handle());
    }
    info.GetReturnValue().Set(result);
}

void Variable::GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int natts;
    call_netcdf(nc_inq_varnatts(obj->parent_id, obj->id, &natts));
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < natts; i++) {
        call_netcdf(nc_inq_attname(obj->parent_id, obj->id, i, name));
        Attribute* a = new Attribute(name, obj->id, obj->parent_id);
        result->Set(v8::String::NewFromUtf8(isolate, name), a->handle());
    }
    info.GetReturnValue().Set(result);
}

void Variable::GetType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    const char* res;
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        res = "Variable type not supported yet";
    } else {
        res = type_names[obj->type];
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res));
}

void Variable::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    char name[NC_MAX_NAME + 1];
    if (obj->get_name(name)) {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name));
    }
}

void Variable::SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    v8::String::Utf8Value new_name_(val->ToString());
    call_netcdf(nc_rename_var(obj->parent_id, obj->id, *new_name_));
}

void Variable::GetEndianness(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    call_netcdf(nc_inq_var_endian(obj->parent_id, obj->id, &v));
    const char* res;
    switch (v) {
        case NC_ENDIAN_LITTLE:
            res = "litte";
            break;
        case NC_ENDIAN_BIG:
            res = "big";
            break;
        case NC_ENDIAN_NATIVE:
            res = "native";
            break;
        default:
            res = "unknown";
            break;
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res));
}

void Variable::SetEndianness(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    std::string arg = *v8::String::Utf8Value(val->ToString());
    int v;
    if (arg == "little") {
        v = NC_ENDIAN_LITTLE;
    } else if (arg == "big") {
        v = NC_ENDIAN_BIG;
    } else if (arg == "native") {
        v = NC_ENDIAN_NATIVE;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value")));
        return;
    }
    call_netcdf(nc_def_var_endian(obj->parent_id, obj->id, v));
}

void Variable::GetChecksumMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    call_netcdf(nc_inq_var_fletcher32(obj->parent_id, obj->id, &v));
    const char* res;
    switch (v) {
        case NC_NOCHECKSUM:
            res = "none";
            break;
        case NC_FLETCHER32:
            res = "fletcher32";
            break;
        default:
            res = "unknown";
            break;
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res));
}

void Variable::SetChecksumMode(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    std::string arg = *v8::String::Utf8Value(val->ToString());
    int v;
    if (arg == "none") {
        v = NC_NOCHECKSUM;
    } else if (arg == "fletcher32") {
        v = NC_FLETCHER32;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value")));
        return;
    }
    call_netcdf(nc_def_var_fletcher32(obj->parent_id, obj->id, v));
}

void Variable::GetChunkMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    call_netcdf(nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL));
    const char* res;
    switch (v) {
        case NC_CONTIGUOUS:
            res = "contiguous";
            break;
        case NC_CHUNKED:
            res = "chunked";
            break;
        default:
            res = "unknown";
            break;
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res));
}

void Variable::SetChunkMode(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    std::string arg = *v8::String::Utf8Value(val->ToString());
    int v;
    if (arg == "contiguous") {
        v = NC_CONTIGUOUS;
    } else if (arg == "chunked") {
        v = NC_CHUNKED;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value")));
        return;
    }
    int len;
    call_netcdf(nc_inq_varndims(obj->parent_id, obj->id, &len));
    size_t sizes[len];
    call_netcdf(nc_inq_var_chunking(obj->parent_id, obj->id, NULL, sizes));
    call_netcdf(nc_def_var_chunking(obj->parent_id, obj->id, v, sizes));
}

void Variable::GetChunkSizes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    size_t sizes[obj->ndims];
    call_netcdf(nc_inq_var_chunking(obj->parent_id, obj->id, NULL, sizes));
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    for (int i = 0; i < obj->ndims; i++) {
        result->Set(i, v8::Integer::New(isolate, i));
    }
    info.GetReturnValue().Set(result);
}

void Variable::SetChunkSizes(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting an array")));
        return;
    }
    v8::Local<v8::Object> array = val->ToObject();
    if (array->Get(v8::String::NewFromUtf8(isolate, "length"))->Int32Value() != obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array size")));
        return;
    }
    int v;
    call_netcdf(nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL));
    size_t sizes[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        sizes[i] = array->Get(i)->Uint32Value();
    }
    call_netcdf(nc_def_var_chunking(obj->parent_id, obj->id, v, sizes));
}

void Variable::GetFillMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, &v, NULL));
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
}

void Variable::SetFillMode(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean")));
        return;
    }
    int v = val->BooleanValue() ? 1 : 0;
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
        return;
    }
    uint8_t value[type_sizes[obj->type]];
    call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, value));
    call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, v, value));
}

void Variable::GetFillValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    v8::Local<v8::Value> result;
    switch (obj->type) {
        case NC_BYTE: {
            int8_t v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_CHAR: {
            char v[2];
            v[1] = 0;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::String::NewFromUtf8(isolate, v);
        } break;
        case NC_SHORT: {
            int16_t v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_INT: {
            int32_t v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_FLOAT: {
            float v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Number::New(isolate, v);
        } break;
        case NC_DOUBLE: {
            double v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Number::New(isolate, v);
        } break;
        case NC_UBYTE: {
            uint8_t v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_USHORT: {
            uint16_t v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_UINT: {
            uint32_t v;
            call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v));
            result = v8::Integer::New(isolate, v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
    info.GetReturnValue().Set(result);
}

void Variable::SetFillValue(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int mode;
    call_netcdf(nc_inq_var_fill(obj->parent_id, obj->id, &mode, NULL));
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR: {
            int8_t v = val->Int32Value();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        case NC_SHORT: {
            int16_t v = val->Int32Value();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        case NC_INT: {
            int32_t v = val->Int32Value();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        case NC_FLOAT: {
            float v = val->NumberValue();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        case NC_DOUBLE: {
            double v = val->NumberValue();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        case NC_UBYTE: {
            uint8_t v = val->Uint32Value();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        case NC_USHORT: {
            uint16_t v = val->Uint32Value();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        case NC_UINT: {
            uint32_t v = val->Uint32Value();
            call_netcdf(nc_def_var_fill(obj->parent_id, obj->id, mode, &v));
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
}

void Variable::GetCompressionShuffle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    call_netcdf(nc_inq_var_deflate(obj->parent_id, obj->id, &v, NULL, NULL));
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
}

void Variable::SetCompressionShuffle(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean")));
        return;
    }
    int v = val->BooleanValue() ? 1 : 0;
    int v1, v2;
    call_netcdf(nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v1, &v2));
    call_netcdf(nc_def_var_deflate(obj->parent_id, obj->id, v, v1, v2));
}

void Variable::GetCompressionDeflate(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    call_netcdf(nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v, NULL));
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
}

void Variable::SetCompressionDeflate(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean")));
        return;
    }
    int v = val->BooleanValue() ? 1 : 0;
    int v1, v2;
    call_netcdf(nc_inq_var_deflate(obj->parent_id, obj->id, &v1, NULL, &v2));
    call_netcdf(nc_def_var_deflate(obj->parent_id, obj->id, v1, v, v2));
}

void Variable::GetCompressionLevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    call_netcdf(nc_inq_var_deflate(obj->parent_id, obj->id, NULL, NULL, &v));
    info.GetReturnValue().Set(v8::Integer::New(isolate, v));
}

void Variable::SetCompressionLevel(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsUint32()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a non-negative integer")));
        return;
    }
    int v = val->IntegerValue();
    int v1, v2;
    call_netcdf(nc_inq_var_deflate(obj->parent_id, obj->id, &v1, &v2, NULL));
    call_netcdf(nc_def_var_deflate(obj->parent_id, obj->id, v1, v2, v));
}

void Variable::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Variable]"));
}
}
