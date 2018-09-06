#include "Variable.h"
#include "Attribute.h"
#include "Dimension.h"
#include "netcdf4js.h"

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
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    Wrap(obj);
    int retval = nc_inq_var(parent_id, id, NULL, &type, &ndims, NULL, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
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
    int retval = nc_inq_varname(parent_id, id, name);
    if (retval != NC_NOERR) {
        throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
        return false;
    }
    return true;
}

void Variable::Write(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[i]->IntegerValue());
        size[i] = 1;
    }
    int retval;
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR: {
            int8_t v = args[obj->ndims]->Int32Value();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_SHORT: {
            int16_t v = args[obj->ndims]->Int32Value();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_INT: {
            int32_t v = args[obj->ndims]->Int32Value();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_FLOAT: {
            float v = static_cast<float>(args[obj->ndims]->NumberValue());
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_DOUBLE: {
            double v = args[obj->ndims]->NumberValue();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_UBYTE: {
            uint8_t v = args[obj->ndims]->Uint32Value();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_USHORT: {
            uint16_t v = args[obj->ndims]->Uint32Value();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_UINT: {
            uint32_t v = args[obj->ndims]->Uint32Value();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            delete[] pos;
            delete[] size;
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] pos;
    delete[] size;
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
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[2 * i]->IntegerValue());
        size_t s = static_cast<size_t>(args[2 * i + 1]->IntegerValue());
        size[i] = s;
        total_size *= s;
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[2 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array")));
        delete[] pos;
        delete[] size;
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
            delete[] pos;
            delete[] size;
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type")));
        delete[] pos;
        delete[] size;
        return;
    }
    int retval = nc_put_vara(obj->parent_id, obj->id, pos, size, val->Buffer()->GetContents().Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] pos;
    delete[] size;
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
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    ptrdiff_t* stride = new ptrdiff_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[3 * i]->IntegerValue());
        size_t s = static_cast<size_t>(args[3 * i + 1]->IntegerValue());
        size[i] = s;
        total_size *= s;
        stride[i] = static_cast<ptrdiff_t>(args[3 * i + 2]->IntegerValue());
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[3 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array")));
        delete[] pos;
        delete[] size;
        delete[] stride;
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
            delete[] pos;
            delete[] size;
            delete[] stride;
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type")));
        delete[] pos;
        delete[] size;
        delete[] stride;
        return;
    }
    int retval = nc_put_vars(obj->parent_id, obj->id, pos, size, stride, val->Buffer()->GetContents().Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] pos;
    delete[] size;
    delete[] stride;
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
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[i]->IntegerValue());
        size[i] = 1;
    }
    v8::Local<v8::Value> result;
    int retval;
    switch (obj->type) {
        case NC_BYTE: {
            int8_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_CHAR: {
            char v[2];
            v[1] = 0;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::String::NewFromUtf8(isolate, v);
        } break;
        case NC_SHORT: {
            int16_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_INT: {
            int32_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_FLOAT: {
            float v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_DOUBLE: {
            double v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_UBYTE: {
            uint8_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_USHORT: {
            uint16_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_UINT: {
            uint32_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    } else {
        args.GetReturnValue().Set(result);
    }
    delete[] pos;
    delete[] size;
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

    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[2 * i]->IntegerValue());
        size_t s = static_cast<size_t>(args[2 * i + 1]->IntegerValue());
        size[i] = s;
        total_size *= s;
    }
    v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, total_size * type_sizes[obj->type]);
    int retval = nc_get_vara(obj->parent_id, obj->id, pos, size, buffer->GetContents().Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] pos;
        delete[] size;
        return;
    }
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
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            delete[] pos;
            delete[] size;
            return;
    }
    args.GetReturnValue().Set(result);
    delete[] pos;
    delete[] size;
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

    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    ptrdiff_t* stride = new ptrdiff_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[3 * i]->IntegerValue());
        size_t s = static_cast<size_t>(args[3 * i + 1]->IntegerValue());
        size[i] = s;
        total_size *= s;
        stride[i] = static_cast<ptrdiff_t>(args[3 * i + 2]->IntegerValue());
    }
    v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, total_size * type_sizes[obj->type]);
    int retval = nc_get_vars(obj->parent_id, obj->id, pos, size, stride, buffer->GetContents().Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] pos;
        delete[] size;
        delete[] stride;
        return;
    }
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
    delete[] pos;
    delete[] size;
    delete[] stride;
}

void Variable::AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    std::string type_str = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        args[1]);
    int type = get_type(type_str);
    if (type == NC2_ERR) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown variable type")));
        return;
    }
    Attribute* res = new Attribute(*v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
                                       isolate,
#endif
                                       args[0]),
                                   obj->id, obj->parent_id, type);
    res->set_value(args[2]);
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
    int* dim_ids = new int[obj->ndims];
    int retval = nc_inq_vardimid(obj->parent_id, obj->id, dim_ids);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] dim_ids;
        return;
    }
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    for (int i = 0; i < obj->ndims; i++) {
        Dimension* d = new Dimension(dim_ids[i], obj->parent_id);
        result->Set(i, d->handle());
    }
    info.GetReturnValue().Set(result);
    delete[] dim_ids;
}

void Variable::GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int natts;
    int retval = nc_inq_varnatts(obj->parent_id, obj->id, &natts);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < natts; i++) {
        retval = nc_inq_attname(obj->parent_id, obj->id, i, name);
        if (retval != NC_NOERR) {
            throw_netcdf_error(isolate, retval);
            return;
        }
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
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    v8::String::Utf8Value new_name_(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString());
    int retval = nc_rename_var(obj->parent_id, obj->id, *new_name_);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
}

void Variable::GetEndianness(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_endian(obj->parent_id, obj->id, &v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
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
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString());
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
    int retval = nc_def_var_endian(obj->parent_id, obj->id, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
}

void Variable::GetChecksumMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_fletcher32(obj->parent_id, obj->id, &v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
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
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString());
    int v;
    if (arg == "none") {
        v = NC_NOCHECKSUM;
    } else if (arg == "fletcher32") {
        v = NC_FLETCHER32;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value")));
        return;
    }
    int retval = nc_def_var_fletcher32(obj->parent_id, obj->id, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
}

void Variable::GetChunkMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
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
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString());
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
    int retval = nc_inq_varndims(obj->parent_id, obj->id, &len);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    size_t* sizes = new size_t[len];
    retval = nc_inq_var_chunking(obj->parent_id, obj->id, NULL, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] sizes;
        return;
    }
    retval = nc_def_var_chunking(obj->parent_id, obj->id, v, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] sizes;
}

void Variable::GetChunkSizes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    size_t* sizes = new size_t[obj->ndims];
    int retval = nc_inq_var_chunking(obj->parent_id, obj->id, NULL, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] sizes;
        return;
    }
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    for (int i = 0; i < obj->ndims; i++) {
        result->Set(i, v8::Integer::New(isolate, i));
    }
    info.GetReturnValue().Set(result);
    delete[] sizes;
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
    int retval = nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    size_t* sizes = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        sizes[i] = array->Get(i)->Uint32Value();
    }
    retval = nc_def_var_chunking(obj->parent_id, obj->id, v, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] sizes;
}

void Variable::GetFillMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_fill(obj->parent_id, obj->id, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
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
    uint8_t* value = new uint8_t[type_sizes[obj->type]];
    int retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, value);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] value;
        return;
    }
    retval = nc_def_var_fill(obj->parent_id, obj->id, v, value);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] value;
}

void Variable::GetFillValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    v8::Local<v8::Value> result;
    int retval;
    switch (obj->type) {
        case NC_BYTE: {
            int8_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_CHAR: {
            char v[2];
            v[1] = 0;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::String::NewFromUtf8(isolate, v);
        } break;
        case NC_SHORT: {
            int16_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_INT: {
            int32_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_FLOAT: {
            float v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_DOUBLE: {
            double v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_UBYTE: {
            uint8_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_USHORT: {
            uint16_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_UINT: {
            uint32_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(result);
}

void Variable::SetFillValue(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int mode;
    int retval = nc_inq_var_fill(obj->parent_id, obj->id, &mode, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR: {
            int8_t v = val->Int32Value();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_SHORT: {
            int16_t v = val->Int32Value();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_INT: {
            int32_t v = val->Int32Value();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_FLOAT: {
            float v = static_cast<float>(val->NumberValue());
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_DOUBLE: {
            double v = val->NumberValue();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_UBYTE: {
            uint8_t v = val->Uint32Value();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_USHORT: {
            uint16_t v = val->Uint32Value();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_UINT: {
            uint32_t v = val->Uint32Value();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet")));
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::GetCompressionShuffle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v, NULL, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
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
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v1, &v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    retval = nc_def_var_deflate(obj->parent_id, obj->id, v, v1, v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::GetCompressionDeflate(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
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
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v1, NULL, &v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    retval = nc_def_var_deflate(obj->parent_id, obj->id, v1, v, v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::GetCompressionLevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, NULL, &v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Integer::New(isolate, v));
}

void Variable::SetCompressionLevel(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsUint32()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a non-negative integer")));
        return;
    }
    int v = static_cast<int>(val->IntegerValue());
    int v1, v2;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v1, &v2, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    retval = nc_def_var_deflate(obj->parent_id, obj->id, v1, v2, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Variable]"));
}
}  // namespace netcdf4js
