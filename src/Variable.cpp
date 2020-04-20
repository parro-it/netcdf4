#include "Variable.h"
#include "Attribute.h"
#include "Utils.h"
// #include "Dimension.h"
#include "napi-utils.h"


namespace netcdf4js {

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

napi_ref Variable::constructor;

Variable::Variable(const int& id_, const int& parent_id_) : id(id_), parent_id(parent_id_) {

}

Variable::~Variable() {
    napi_delete_reference(env_, wrapper_);
}

extern "C" {
    napi_value Variable_Init(napi_env env, napi_value exports) {
        return Variable::Init(env, exports);
    }
}

void Variable::Destructor(napi_env env, void* nativeObject, void* finalize_hint) {
    reinterpret_cast<Variable*>(nativeObject)->~Variable();
}

napi_value Variable::Init(napi_env env, napi_value exports) {
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_METHOD("read", Read),
        DECLARE_NAPI_METHOD("readSlice", ReadSlice),
        DECLARE_NAPI_METHOD("readStridedSlice", ReadStridedSlice),
        DECLARE_NAPI_METHOD("write", Write),
        DECLARE_NAPI_METHOD("writeSlice", WriteSlice),
        DECLARE_NAPI_METHOD("writeStridedSlice", WriteStridedSlice),
        DECLARE_NAPI_METHOD("addAttribute", AddAttribute),
        DECLARE_NAPI_METHOD("inspect", Inspect),

        DECLARE_NAPI_PROP("id", Variable::GetId, nullptr),
        DECLARE_NAPI_PROP("type", Variable::GetType, nullptr),
        DECLARE_NAPI_PROP("dimensions", Variable::GetDimensions, nullptr),
        DECLARE_NAPI_PROP("attributes", Variable::GetAttributes, nullptr),
        DECLARE_NAPI_PROP("name", Variable::GetName, Variable::SetName),
        DECLARE_NAPI_PROP("endianness", Variable::GetEndianness, Variable::SetEndianness),
        DECLARE_NAPI_PROP("checksummode", Variable::GetChecksumMode, Variable::SetChecksumMode),
        DECLARE_NAPI_PROP("chunkmode", Variable::GetChunkMode, Variable::SetChunkMode),
        DECLARE_NAPI_PROP("chunksizes", Variable::GetChunkSizes, Variable::SetChunkSizes),
        DECLARE_NAPI_PROP("fillmode", Variable::GetFillMode, Variable::SetFillMode),
        DECLARE_NAPI_PROP("fillvalue", Variable::GetFillValue, Variable::SetFillValue),
        DECLARE_NAPI_PROP("compressionshuffle", Variable::GetCompressionShuffle, Variable::SetCompressionShuffle),
        DECLARE_NAPI_PROP("compressiondeflate", Variable::GetCompressionDeflate, Variable::SetCompressionDeflate),
        DECLARE_NAPI_PROP("compressionlevel", Variable::GetCompressionLevel, Variable::SetCompressionLevel),
    };

    napi_value cons;

    NAPI_CALL(napi_define_class(
        env,
        "Variable", NAPI_AUTO_LENGTH,
        Variable::New,
        nullptr,
        22,
        properties,
        &cons
    ));

    NAPI_CALL(napi_set_named_property(
        env,
        exports,
        "Variable",
        cons
    ));

    NAPI_CALL(napi_create_reference(
        env,
        cons,
        0,
        &constructor
    ));

    return nullptr;
}


napi_value Variable::Build(napi_env env, int id, int parent_id) {
    napi_value var_js;
    char* nuts_error = NULL;

    VAR_JS_FROM_I32(id_js, id);
    VAR_JS_FROM_I32(parent_id_js, parent_id);

    napi_value cons;

    NAPI_CALL(napi_get_reference_value(env, constructor, &cons));
    napi_value args[] = {id_js, parent_id_js};
    NAPI_CALL(napi_new_instance(env, cons, 2, args ,&var_js));

    return var_js;
}

napi_value Variable::New(napi_env env, napi_callback_info info) {
    ARGS(2, I32(id), I32(parent_id))
    Variable* var = new Variable(id, parent_id);
    var->env_ = env;

    NAPI_CALL(napi_wrap(
        env,
        jsthis,
        reinterpret_cast<void*>(var),
        Variable::Destructor,
        nullptr,  // finalize_hint
        &var->wrapper_
    ));

    NC_CALL(nc_inq_var(parent_id, id, NULL, &var->type, &var->ndims, NULL, NULL));

    return jsthis;
}

bool Variable::get_name(char* name) const {
    int retval = nc_inq_varname(parent_id, id, name);
    if (retval != NC_NOERR) {
        // throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
        return false;
    }
    return true;
}

napi_value Variable::Write(napi_env env, napi_callback_info info) {
    /*

    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = 1;
    }
    int retval;
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR: {
            int8_t v = args[obj->ndims]->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_SHORT: {
            int16_t v = args[obj->ndims]->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_INT: {
            int32_t v = args[obj->ndims]->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_FLOAT: {
            float v = static_cast<float>(args[obj->ndims]->NumberValue(isolate->GetCurrentContext()).ToChecked());
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_DOUBLE: {
            double v = args[obj->ndims]->NumberValue(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_UBYTE: {
            uint8_t v = args[obj->ndims]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_USHORT: {
            uint16_t v = args[obj->ndims]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_UINT: {
            uint32_t v = args[obj->ndims]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            delete[] pos;
            delete[] size;
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] pos;
    delete[] size;
    */ return NULL;
}

napi_value Variable::WriteSlice(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != 2 * obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (!args[2 * obj->ndims]->IsTypedArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a typed array", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[2 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size_t s = static_cast<size_t>(args[2 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = s;
        total_size *= s;
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[2 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array", v8::NewStringType::kNormal).ToLocalChecked()));
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
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            delete[] pos;
            delete[] size;
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type", v8::NewStringType::kNormal).ToLocalChecked()));
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
    */ return NULL;
}

napi_value Variable::WriteStridedSlice(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != 3 * obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (!args[3 * obj->ndims]->IsTypedArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a typed array", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    ptrdiff_t* stride = new ptrdiff_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[3 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size_t s = static_cast<size_t>(args[3 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = s;
        total_size *= s;
        stride[i] = static_cast<ptrdiff_t>(args[3 * i + 2]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[3 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array", v8::NewStringType::kNormal).ToLocalChecked()));
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
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            delete[] pos;
            delete[] size;
            delete[] stride;
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type", v8::NewStringType::kNormal).ToLocalChecked()));
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
    delete[] stride;*/ return NULL;
}

napi_value Variable::Read(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() != obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
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
            result = v8::String::NewFromUtf8(isolate, v, v8::NewStringType::kNormal).ToLocalChecked();
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
    delete[] size;*/ return NULL;
}



napi_value Variable::ReadSlice(napi_env env, napi_callback_info info) {
    napi_value argv[256];
    size_t argc = 256;
    char* nuts_error = NULL;
    napi_value jsthis;

    NAPI_CALL(napi_get_cb_info(env, info, &argc, argv, &jsthis, NULL));

    Variable* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));


	if ((int)argc != self->ndims * 2) {
		napi_throw_error(env, NULL, "Wrong number of arguments");
		return NULL;
	}

    if (self->type < NC_BYTE || self->type > NC_UINT) {
        napi_throw_error(env, NULL, "Variable type not supported yet");
        return NULL;
    }

    size_t pos[self->ndims];
    size_t size[self->ndims];
    size_t total_size = 1;

    for (int i = 0; i < self->ndims; i++) {
        VAR_I32_FROM_JS(pos_i, argv[2* i]);
        VAR_I32_FROM_JS(size_i, argv[2* i + 1]);

        pos[i] = pos_i;
        size[i] = size_i;
        total_size *= size_i;
    }



    void* buffer_data = NULL;
    napi_value buffer;

    NAPI_CALL(napi_create_arraybuffer(
        env,
        total_size * type_sizes[self->type],
        &buffer_data,
        &buffer
    ));


    NC_CALL(nc_get_vara(
        self->parent_id,
        self->id,
        pos,
        size,
        buffer_data
    ));


    napi_typedarray_type type = typedarray_cons_per_type(env, self->type);


    napi_value result;

    NAPI_CALL(napi_create_typedarray(
        env,
        type,
        total_size,
        buffer,
        0,
        &result
    ));


    return result;



}

napi_value Variable::ReadStridedSlice(napi_env env, napi_callback_info info) {
    napi_value argv[256];
    size_t argc = 256;
    char* nuts_error = NULL;
    napi_value jsthis;

    NAPI_CALL(napi_get_cb_info(env, info, &argc, argv, &jsthis, NULL));

    Variable* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    if ((int)argc != self->ndims * 3) {
		napi_throw_error(env, NULL, "Wrong number of arguments");
		return NULL;
	}

    if (self->type < NC_BYTE || self->type > NC_UINT) {
        napi_throw_error(env, NULL, "Variable type not supported yet");
        return NULL;
    }

    size_t pos[self->ndims];
    size_t size[self->ndims];
    ptrdiff_t stride[self->ndims];
    size_t total_size = 1;

    for (int i = 0; i < self->ndims; i++) {
        VAR_I32_FROM_JS(pos_i, argv[2* i]);
        VAR_I32_FROM_JS(size_i, argv[2* i + 1]);
        VAR_I32_FROM_JS(stride_i, argv[2* i + 2]);

        pos[i] = pos_i;
        size[i] = size_i;
        stride[i] = stride_i;

        total_size *= size_i;
    }

    void* buffer_data = NULL;
    napi_value buffer;

    NAPI_CALL(napi_create_arraybuffer(
        env,
        total_size * type_sizes[self->type],
        &buffer_data,
        &buffer
    ));

    NC_CALL(nc_get_vars(
        self->parent_id,
        self->id,
        pos,
        size,
        stride,
        buffer_data
    ));

    napi_typedarray_type type = typedarray_cons_per_type(env, self->type);

    napi_value result;

    NAPI_CALL(napi_create_typedarray(
        env,
        type,
        total_size,
        buffer,
        0,
        &result
    ));

    return result;
}

napi_value Variable::AddAttribute(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    std::string type_str = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        args[1]);
    int type = get_type(type_str);
    if (type == NC2_ERR) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown variable type", v8::NewStringType::kNormal).ToLocalChecked()));
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
*/ return NULL;
}

napi_value Variable::GetId(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
*/ return NULL;
}

napi_value Variable::GetDimensions(napi_env env, napi_callback_info info) {
    /*
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
        result->Set(isolate->GetCurrentContext(), i, d->handle());
    }
    info.GetReturnValue().Set(result);
    delete[] dim_ids;
*/ return NULL;
}

napi_value Variable::GetAttributes(napi_env env, napi_callback_info info) {
    ARGS(0);
    Variable* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    int natts;
    NC_CALL(nc_inq_varnatts(self->parent_id, self->id, &natts));

    napi_value array;
    NAPI_CALL(napi_create_array_with_length(env, natts, &array));

    for (int i = 0; i < natts; i++) {
        char name[NC_MAX_NAME + 1];
        nc_type type;

        NC_CALL(nc_inq_attname(self->parent_id, self->id, i, name));
        NC_CALL(nc_inq_atttype(self->parent_id, self->id, name, &type));

        napi_value attr = Attribute::Build(env, name, self->id, self->parent_id, type);
        NAPI_CALL(napi_set_element(env,array,i, attr));
        NAPI_CALL(napi_set_named_property(env,array,name, attr));
    }

    return array;


    /*
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
        result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(), a->handle());
    }
    info.GetReturnValue().Set(result);
*/ return NULL;
}

napi_value Variable::GetType(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    const char* res;
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        res = "Variable type not supported yet";
    } else {
        res = type_names[obj->type];
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
*/ return NULL;
}

napi_value Variable::GetName(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    char name[NC_MAX_NAME + 1];
    if (obj->get_name(name)) {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked());
    }
*/ return NULL;
}

napi_value Variable::SetName(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    v8::String::Utf8Value new_name_(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int retval = nc_rename_var(obj->parent_id, obj->id, *new_name_);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
*/ return NULL;
}

napi_value Variable::GetEndianness(napi_env env, napi_callback_info info) {
    /*
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
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
*/ return NULL;
}

napi_value Variable::SetEndianness(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int v;
    if (arg == "little") {
        v = NC_ENDIAN_LITTLE;
    } else if (arg == "big") {
        v = NC_ENDIAN_BIG;
    } else if (arg == "native") {
        v = NC_ENDIAN_NATIVE;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int retval = nc_def_var_endian(obj->parent_id, obj->id, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
*/ return NULL;
}

napi_value Variable::GetChecksumMode(napi_env env, napi_callback_info info) {
    /*
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
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
*/ return NULL;
}

napi_value Variable::SetChecksumMode(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int v;
    if (arg == "none") {
        v = NC_NOCHECKSUM;
    } else if (arg == "fletcher32") {
        v = NC_FLETCHER32;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int retval = nc_def_var_fletcher32(obj->parent_id, obj->id, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
*/ return NULL;
}

napi_value Variable::GetChunkMode(napi_env env, napi_callback_info info) {
    /*
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
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
*/ return NULL;
}

napi_value Variable::SetChunkMode(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int v;
    if (arg == "contiguous") {
        v = NC_CONTIGUOUS;
    } else if (arg == "chunked") {
        v = NC_CHUNKED;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal).ToLocalChecked()));
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
*/ return NULL;
}

napi_value Variable::GetChunkSizes(napi_env env, napi_callback_info info) {
    /*
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
        result->Set(isolate->GetCurrentContext(), i, v8::Integer::New(isolate, i));
    }
    info.GetReturnValue().Set(result);
    delete[] sizes;
*/ return NULL;
}

napi_value Variable::SetChunkSizes(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting an array", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    v8::Local<v8::Object> array = val->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    if (array->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->Int32Value(isolate->GetCurrentContext()).ToChecked() != obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array size", v8::NewStringType::kNormal).ToLocalChecked()));
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
        sizes[i] = array->Get(isolate->GetCurrentContext(), i).ToLocalChecked()->Uint32Value(isolate->GetCurrentContext()).ToChecked();
    }
    retval = nc_def_var_chunking(obj->parent_id, obj->id, v, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] sizes;
*/ return NULL;
}

napi_value Variable::GetFillMode(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_fill(obj->parent_id, obj->id, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
*/ return NULL;
}

napi_value Variable::SetFillMode(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
#if NODE_MAJOR_VERSION > 11
    int v = val->BooleanValue(isolate) ? 1 : 0;
#else
    int v = val->BooleanValue() ? 1 : 0;
#endif
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
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
*/ return NULL;
}

napi_value Variable::GetFillValue(napi_env env, napi_callback_info info) {
    /*
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
            result = v8::String::NewFromUtf8(isolate, v, v8::NewStringType::kNormal).ToLocalChecked();
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
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(result);
*/ return NULL;
}

napi_value Variable::SetFillValue(napi_env env, napi_callback_info info) {
    /*
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
            int8_t v = val->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_SHORT: {
            int16_t v = val->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_INT: {
            int32_t v = val->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_FLOAT: {
            float v = static_cast<float>(val->NumberValue(isolate->GetCurrentContext()).ToChecked());
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_DOUBLE: {
            double v = val->NumberValue(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_UBYTE: {
            uint8_t v = val->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_USHORT: {
            uint16_t v = val->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_UINT: {
            uint32_t v = val->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
*/ return NULL;
}

napi_value Variable::GetCompressionShuffle(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v, NULL, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
*/ return NULL;
}

napi_value Variable::SetCompressionShuffle(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
#if NODE_MAJOR_VERSION > 11
    int v = val->BooleanValue(isolate) ? 1 : 0;
#else
    int v = val->BooleanValue() ? 1 : 0;
#endif
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
*/ return NULL;
}

napi_value Variable::GetCompressionDeflate(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
*/ return NULL;
}

napi_value Variable::SetCompressionDeflate(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
#if NODE_MAJOR_VERSION > 11
    int v = val->BooleanValue(isolate) ? 1 : 0;
#else
    int v = val->BooleanValue() ? 1 : 0;
#endif
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
*/ return NULL;
}

napi_value Variable::GetCompressionLevel(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, NULL, &v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Integer::New(isolate, v));
*/ return NULL;
}

napi_value Variable::SetCompressionLevel(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
    if (!val->IsUint32()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a non-negative integer", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int v = static_cast<int>(val->IntegerValue(isolate->GetCurrentContext()).ToChecked());
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
*/ return NULL;
}

napi_value Variable::Inspect(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Variable]", v8::NewStringType::kNormal).ToLocalChecked());
*/ return NULL;
}
}  // namespace netcdf4js
