#include <netcdf.h>
#include "netcdf4js.h"
#include "netcdf4jstypes.h"

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
	4  // NC_UINT
};


Napi::FunctionReference Variable::constructor;

Napi::Object Variable::Build(Napi::Env env, int id, int parent_id) {
	return constructor.New({Napi::Number::New(env, id), Napi::Number::New(env, parent_id)});
}

Variable::Variable(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Variable>(info) {
	if (info.Length() < 2) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return;
	}

	char varName[NC_MAX_NAME + 1];
	id = info[0].As<Napi::Number>().Int32Value();
	parent_id = info[1].As<Napi::Number>().Int32Value();
	NC_CALL_VOID(nc_inq_var(parent_id, id, varName, &type, &ndims, NULL, NULL));
	name = std::string(varName);
}

Napi::Object Variable::Init(Napi::Env env, Napi::Object exports) {
	Napi::HandleScope scope(env);

	auto properties = {
		InstanceMethod("read", &Variable::Read),
		InstanceMethod("readSlice", &Variable::ReadSlice),
		InstanceMethod("readStridedSlice", &Variable::ReadStridedSlice),
		InstanceMethod("write", &Variable::Write),
		InstanceMethod("writeSlice", &Variable::WriteSlice),
		InstanceMethod("writeStridedSlice", &Variable::WriteStridedSlice),
		InstanceMethod("addAttribute", &Variable::AddAttribute),
		InstanceMethod("inspect", &Variable::Inspect),

		InstanceAccessor<&Variable::GetId>("id"),
		InstanceAccessor<&Variable::GetType>("type"),
		InstanceAccessor<&Variable::GetDimensions>("dimensions"),
		InstanceAccessor<&Variable::GetAttributes>("attributes"),
		InstanceAccessor<&Variable::GetName, &Variable::SetName>("name"),
		InstanceAccessor<&Variable::GetEndianness, &Variable::SetEndianness>("endianness"),
		InstanceAccessor<&Variable::GetChecksumMode, &Variable::SetChecksumMode>("checksummode"),
		InstanceAccessor<&Variable::GetChunkMode, &Variable::SetChunkMode>("chunkmode"),
		InstanceAccessor<&Variable::GetChunkSizes, &Variable::SetChunkSizes>("chunksizes"),
		InstanceAccessor<&Variable::GetFillMode, &Variable::SetFillMode>("fillmode"),
		InstanceAccessor<&Variable::GetFillValue, &Variable::SetFillValue>("fillvalue"),
		InstanceAccessor<&Variable::GetCompressionShuffle, &Variable::SetCompressionShuffle>(
			"compressionshuffle"),
		InstanceAccessor<&Variable::GetCompressionDeflate, &Variable::SetCompressionDeflate>(
			"compressiondeflate"),
		InstanceAccessor<&Variable::GetCompressionLevel, &Variable::SetCompressionLevel>(
			"compressionlevel"),

	};

	Napi::Function func = DefineClass(env, "Variable", properties);
	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Variable", func);
	return exports;
}
Napi::Value Variable::Write(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
	if (args.Length() != obj->ndims + 1) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	size_t *pos = new size_t[obj->ndims];
	size_t *size = new size_t[obj->ndims];
	for (int i = 0; i < obj->ndims; i++) {
		pos[i] =
			static_cast<size_t>(args[i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
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
		float v = static_cast<float>(
			args[obj->ndims]->NumberValue(isolate->GetCurrentContext()).ToChecked());
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
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
		delete[] pos;
		delete[] size;
		return;
	}
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
	}
	delete[] pos;
	delete[] size;
	*/
	return info.Env().Undefined();
}

Napi::Value Variable::WriteSlice(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
	if (args.Length() != 2 * obj->ndims + 1) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	if (!args[2 * obj->ndims]->IsTypedArray()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting a typed array", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	size_t *pos = new size_t[obj->ndims];
	size_t *size = new size_t[obj->ndims];
	size_t total_size = 1;
	for (int i = 0; i < obj->ndims; i++) {
		pos[i] = static_cast<size_t>(
			args[2 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
		size_t s = static_cast<size_t>(
			args[2 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
		size[i] = s;
		total_size *= s;
	}
	v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[2 * obj->ndims]);
	if (val->Length() != total_size) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Wrong size of array", v8::NewStringType::kNormal)
				.ToLocalChecked()));
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
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
		delete[] pos;
		delete[] size;
		return;
	}
	if (!correct_type) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Wrong array type", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		delete[] pos;
		delete[] size;
		return;
	}
	int retval =
		nc_put_vara(obj->parent_id, obj->id, pos, size, val->Buffer()->GetContents().Data());
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
	}
	delete[] pos;
	delete[] size;
	*/
	return info.Env().Undefined();
}

Napi::Value Variable::WriteStridedSlice(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
	if (args.Length() != 3 * obj->ndims + 1) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	if (!args[3 * obj->ndims]->IsTypedArray()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting a typed array", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	size_t *pos = new size_t[obj->ndims];
	size_t *size = new size_t[obj->ndims];
	ptrdiff_t *stride = new ptrdiff_t[obj->ndims];
	size_t total_size = 1;
	for (int i = 0; i < obj->ndims; i++) {
		pos[i] = static_cast<size_t>(
			args[3 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
		size_t s = static_cast<size_t>(
			args[3 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
		size[i] = s;
		total_size *= s;
		stride[i] = static_cast<ptrdiff_t>(
			args[3 * i + 2]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
	}
	v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[3 * obj->ndims]);
	if (val->Length() != total_size) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Wrong size of array", v8::NewStringType::kNormal)
				.ToLocalChecked()));
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
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
		delete[] pos;
		delete[] size;
		delete[] stride;
		return;
	}
	if (!correct_type) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Wrong array type", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		delete[] pos;
		delete[] size;
		delete[] stride;
		return;
	}
	int retval = nc_put_vars(obj->parent_id, obj->id, pos, size, stride,
							 val->Buffer()->GetContents().Data());
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
	}
	delete[] pos;
	delete[] size;
	delete[] stride;
	*/
	return info.Env().Undefined();
}

Napi::Value Variable::Read(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
	if (args.Length() != obj->ndims) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	if (obj->type < NC_BYTE || obj->type > NC_UINT) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	size_t *pos = new size_t[obj->ndims];
	size_t *size = new size_t[obj->ndims];
	for (int i = 0; i < obj->ndims; i++) {
		pos[i] =
			static_cast<size_t>(args[i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
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
	delete[] size;
	*/
	return info.Env().Undefined();
}

Napi::Value Variable::ReadSlice(const Napi::CallbackInfo &info) {
	if (info.Length() != static_cast<size_t>(2 * this->ndims)) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}

	if (this->type < NC_BYTE || this->type > NC_UINT) {
		Napi::TypeError::New(info.Env(), "Variable type not supported yet")
			.ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}

	size_t *pos = new size_t[this->ndims];
	size_t *size = new size_t[this->ndims];
	size_t total_size = 1;

	for (int i = 0; i < this->ndims; i++) {
		pos[i] = info[2 * i].As<Napi::Number>().Int64Value();
		size[i] = info[2 * i + 1].As<Napi::Number>().Int64Value();
		total_size *= size[i];
	}

	Napi::ArrayBuffer buffer;
	Napi::Value result;

	switch (this->type) {
	case NC_BYTE:
	case NC_CHAR: {
		auto array = Napi::Int8Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_SHORT: {
		auto array = Napi::Int16Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_INT: {
		auto array = Napi::Int32Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_FLOAT: {
		auto array = Napi::Float32Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_DOUBLE: {
		auto array = Napi::Float64Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_UBYTE: {
		auto array = Napi::Uint8Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_USHORT: {
		auto array = Napi::Uint16Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_UINT: {
		auto array = Napi::Uint32Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	}

	NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, buffer.Data()));

	delete[] pos;
	delete[] size;
	return result;
}
Napi::Value Variable::ReadStridedSlice(const Napi::CallbackInfo &info) {
	if (info.Length() != static_cast<size_t>(3 * this->ndims)) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}

	if (this->type < NC_BYTE || this->type > NC_UINT) {
		Napi::TypeError::New(info.Env(), "Variable type not supported yet")
			.ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}

	size_t *pos = new size_t[this->ndims];
	size_t *size = new size_t[this->ndims];
	ptrdiff_t *stride = new ptrdiff_t[this->ndims];
	size_t total_size = 1;

	for (int i = 0; i < this->ndims; i++) {
		pos[i] = info[3 * i].As<Napi::Number>().Int64Value();
		size[i] = info[3 * i + 1].As<Napi::Number>().Int64Value();
		total_size *= size[i];
		stride[i] = static_cast<ptrdiff_t>(
			info[3 * i + 2].As<Napi::Number>().Int64Value()
		);
	}

	Napi::ArrayBuffer buffer;
	Napi::Value result;

	switch (this->type) {
	case NC_BYTE:
	case NC_CHAR: {
		auto array = Napi::Int8Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_SHORT: {
		auto array = Napi::Int16Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_INT: {
		auto array = Napi::Int32Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_FLOAT: {
		auto array = Napi::Float32Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_DOUBLE: {
		auto array = Napi::Float64Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_UBYTE: {
		auto array = Napi::Uint8Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_USHORT: {
		auto array = Napi::Uint16Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	case NC_UINT: {
		auto array = Napi::Uint32Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
	} break;
	}

	NC_CALL(nc_get_vars(this->parent_id, this->id, pos, size, stride, buffer.Data()));

	delete[] pos;
	delete[] size;
	delete[] stride;
	return result;
}

Napi::Value Variable::AddAttribute(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(args.Holder());
	if (args.Length() < 3) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of
arguments", v8::NewStringType::kNormal) .ToLocalChecked())); return;
	}
	std::string type_str = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
		isolate,
#endif
		args[1]);
	int type = get_type(type_str);
	if (type == NC2_ERR) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unknown variable type",
v8::NewStringType::kNormal) .ToLocalChecked())); return;
	}
	Attribute *res = new Attribute(*v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
									   isolate,
#endif
									   args[0]),
								   obj->id, obj->parent_id, type);
	res->set_value(args[2]);
	args.GetReturnValue().Set(res->handle());
	*/
	return info.Env().Undefined();
}

Napi::Value Variable::GetId(const Napi::CallbackInfo &info) {
	return Napi::Number::New(info.Env(), this->id);
}

Napi::Value Variable::GetDimensions(const Napi::CallbackInfo &info) {

	int *dim_ids = new int[this->ndims];
	NC_CALL(nc_inq_vardimid(this->parent_id, this->id, dim_ids));
	Napi::Array result = Napi::Array::New(info.Env());
	for (int i = 0; i < this->ndims; i++) {
		Napi::Object dim = Dimension::Build(info.Env(), this->parent_id, dim_ids[i]);
		result.Set(i,dim);
	}
	delete[] dim_ids;
	return result;
}

Napi::Value Variable::GetAttributes(const Napi::CallbackInfo &info) {
	int natts;
	NC_CALL(nc_inq_varnatts(this->parent_id, this->id, &natts));

	Napi::Object attrs = Napi::Object::New(info.Env());

	char name[NC_MAX_NAME + 1];
	for (int i = 0; i < natts; ++i) {
		NC_CALL(nc_inq_attname(this->parent_id, this->id, i, name));
		Napi::Object attr = Attribute::Build(info.Env(), name, this->id, this->parent_id);
		attrs.Set(name, attr);
	}

	return attrs;
}

Napi::Value Variable::GetType(const Napi::CallbackInfo &info) {
	const char *res=NC_TYPES(this->type);
	return Napi::String::New(info.Env(),res);

}

Napi::Value Variable::GetName(const Napi::CallbackInfo &info) {
//	char name[NC_MAX_NAME + 1];
//	NC_CALL(nc_inq_varname(this->parent_id, this->id, name));
	return Napi::String::New(info.Env(), name);
}

void Variable::SetName(const Napi::CallbackInfo &info, const Napi::Value &value) {
	std::string new_name = value.As<Napi::String>().ToString();
	NC_CALL_VOID(
		nc_rename_var(this->parent_id, this->id,  new_name.c_str())
	);
	this->name = new_name;
}

Napi::Value Variable::GetEndianness(const Napi::CallbackInfo &info) {
	const char *res;
	int v;
	NC_CALL(nc_inq_var_endian(this->parent_id, this->id, &v));
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
	return Napi::String::New(info.Env(), res);
}

void Variable::SetEndianness(const Napi::CallbackInfo &info, const Napi::Value &value) {
	std::string arg=value.As<Napi::String>().ToString();
	int v;
	if (arg == "little") {
		v = NC_ENDIAN_LITTLE;
	} else if (arg == "big") {
		v = NC_ENDIAN_BIG;
	} else if (arg == "native") {
		v = NC_ENDIAN_NATIVE;
	} else {
		Napi::Error::New(info.Env(), "Unknown value").ThrowAsJavaScriptException();        
		return;
	}
	NC_CALL_VOID(nc_def_var_endian(this->parent_id,this->id,v))
}

Napi::Value Variable::GetChecksumMode(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	int v;
	int retval = nc_inq_var_fletcher32(obj->parent_id, obj->id, &v);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	const char *res;
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
	info.GetReturnValue().Set(
		v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
	*/
	return info.Env().Undefined();
}

void Variable::SetChecksumMode(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
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
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	int retval = nc_def_var_fletcher32(obj->parent_id, obj->id, v);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	*/
}

Napi::Value Variable::GetChunkMode(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	int v;
	int retval = nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	const char *res;
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
	info.GetReturnValue().Set(
		v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
	*/
	return info.Env().Undefined();
}

void Variable::SetChunkMode(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
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
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	int len;
	int retval = nc_inq_varndims(obj->parent_id, obj->id, &len);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	size_t *sizes = new size_t[len];
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
	*/
}

Napi::Value Variable::GetChunkSizes(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	size_t *sizes = new size_t[obj->ndims];
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
	*/
	return info.Env().Undefined();
}

void Variable::SetChunkSizes(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	if (!val->IsArray()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting an array", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	v8::Local<v8::Object> array = val->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
	if (array
			->Get(isolate->GetCurrentContext(),
				  v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal)
					  .ToLocalChecked())
			.ToLocalChecked()
			->Int32Value(isolate->GetCurrentContext())
			.ToChecked() != obj->ndims) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Wrong array size", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	int v;
	int retval = nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	size_t *sizes = new size_t[obj->ndims];
	for (int i = 0; i < obj->ndims; i++) {
		sizes[i] = array->Get(isolate->GetCurrentContext(), i)
					   .ToLocalChecked()
					   ->Uint32Value(isolate->GetCurrentContext())
					   .ToChecked();
	}
	retval = nc_def_var_chunking(obj->parent_id, obj->id, v, sizes);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
	}
	delete[] sizes;
	*/
}

Napi::Value Variable::GetFillMode(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	int v;
	int retval = nc_inq_var_fill(obj->parent_id, obj->id, &v, NULL);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
	*/
	return info.Env().Undefined();
}

void Variable::SetFillMode(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	if (!val->IsBoolean()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
#if NODE_MAJOR_VERSION > 11
	int v = val->BooleanValue(isolate) ? 1 : 0;
#else
	int v = val->BooleanValue() ? 1 : 0;
#endif
	if (obj->type < NC_BYTE || obj->type > NC_UINT) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	uint8_t *value = new uint8_t[type_sizes[obj->type]];
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
	*/
}

Napi::Value Variable::GetFillValue(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
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
		result = v8::String::NewFromUtf8(isolate, v,
	v8::NewStringType::kNormal).ToLocalChecked(); } break; case NC_SHORT: { int16_t v; retval =
	nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v); result = v8::Integer::New(isolate, v);
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
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	info.GetReturnValue().Set(result);
	*/
	return info.Env().Undefined();
}

void Variable::SetFillValue(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
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
		float v =
	static_cast<float>(val->NumberValue(isolate->GetCurrentContext()).ToChecked()); retval =
	nc_def_var_fill(obj->parent_id, obj->id, mode, &v); } break; case NC_DOUBLE: { double v =
	val->NumberValue(isolate->GetCurrentContext()).ToChecked(); retval =
	nc_def_var_fill(obj->parent_id, obj->id, mode, &v); } break; case NC_UBYTE: { uint8_t v =
	val->Uint32Value(isolate->GetCurrentContext()).ToChecked(); retval =
	nc_def_var_fill(obj->parent_id, obj->id, mode, &v); } break; case NC_USHORT: { uint16_t v =
	val->Uint32Value(isolate->GetCurrentContext()).ToChecked(); retval =
	nc_def_var_fill(obj->parent_id, obj->id, mode, &v); } break; case NC_UINT: { uint32_t v =
	val->Uint32Value(isolate->GetCurrentContext()).ToChecked(); retval =
	nc_def_var_fill(obj->parent_id, obj->id, mode, &v); } break; default:
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
	}
	* / return info.Env().Undefined();
	*/
}

Napi::Value Variable::GetCompressionShuffle(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	int v;
	int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v, NULL, NULL);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
	*/
	return info.Env().Undefined();
}

void Variable::SetCompressionShuffle(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	if (!val->IsBoolean()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal)
				.ToLocalChecked()));
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
	*/
}

Napi::Value Variable::GetCompressionDeflate(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	int v;
	int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v, NULL);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
	*/
	return info.Env().Undefined();
}

void Variable::SetCompressionDeflate(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	if (!val->IsBoolean()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal)
				.ToLocalChecked()));
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
	*/
}

Napi::Value Variable::GetCompressionLevel(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	int v;
	int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, NULL, &v);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	info.GetReturnValue().Set(v8::Integer::New(isolate, v));
	*/
	return info.Env().Undefined();
}

void Variable::SetCompressionLevel(const Napi::CallbackInfo &info, const Napi::Value &value) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Variable *obj = node::ObjectWrap::Unwrap<Variable>(info.Holder());
	if (!val->IsUint32()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting a non-negative integer",
									v8::NewStringType::kNormal)
				.ToLocalChecked()));
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
	*/
}

Napi::Value Variable::Inspect(const Napi::CallbackInfo &info) {
	return Napi::String::New(info.Env(), 
		string_format(
			"[Variable %s, type %s, %i dimension(s)]",
			this->name.c_str(),
			NC_TYPES(this->type),
			this->ndims)
	);
}
} // namespace netcdf4js
