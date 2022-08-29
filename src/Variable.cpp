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
	if (info.Length() != static_cast<size_t>(this->ndims + 1)) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	size_t *pos = new size_t[this->ndims];
	size_t *size = new size_t[this->ndims];
	for (int i = 0; i < this->ndims; i++) {
		pos[i] =info[i].As<Napi::Number>().Int64Value();
		size[i] = 1;
	}
	switch (this->type) {
	case NC_BYTE:
	case NC_CHAR: {
		int8_t v = info[this->ndims].As<Napi::Number>().Int32Value();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
	case NC_SHORT: {
		int16_t v = info[this->ndims].As<Napi::Number>().Int32Value();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
	case NC_INT: {
		int32_t v = info[this->ndims].As<Napi::Number>().Int32Value();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
	case NC_FLOAT: {
		float v = info[this->ndims].As<Napi::Number>().FloatValue();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
	case NC_DOUBLE: {
		double v = info[this->ndims].As<Napi::Number>().DoubleValue();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
	case NC_UBYTE: {
		uint8_t v = info[this->ndims].As<Napi::Number>().Uint32Value();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
	case NC_USHORT: {
		uint16_t v = info[this->ndims].As<Napi::Number>().Uint32Value();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
	case NC_UINT: {
		uint32_t v = info[this->ndims].As<Napi::Number>().Uint32Value();
		NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, &v));
	} break;
#if NODE_MAJOR_VERSION >= 10
	case NC_UINT64: {
		if (value.IsNumber()) {
			uint64_t v = value.As<Napi::Number>().Uint32Value();
			NC_CALL_VOID(nc_put_vara(this->parent_id, this->id, pos, size, &v));
		} else if (value.IsBigInt()) {
			uint64_t v = value.As<Napi::BigInt>().Uint64Value(nullptr);
			NC_CALL_VOID(nc_put_vara(this->parent_id, this->id, pos, size, &v));
		} else {
			auto array = value.As<Napi::BigUint64Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_vara(this->parent_id, this->id, pos, size, &v));
		}
	} 
	break;
	case NC_INT64: {
		if (value.IsNumber()) {
			int64_t v = value.As<Napi::Number>().Uint32Value();
			NC_CALL_VOID(nc_put_vara(this->parent_id, this->id, pos, size, &v));
		} else if (value.IsBigInt()) {
			int64_t v = value.As<Napi::BigInt>().Int64Value(nullptr);
			NC_CALL_VOID(nc_put_vara(this->parent_id, this->id, pos, size, &v));
		} else {
			auto array = value.As<Napi::BigInt64Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_vara(this->parent_id, this->id, pos, size, &v));
		}
	} 
	break;
#endif
	default: {
		Napi::TypeError::New(info.Env(), "Variable type not supported yet").ThrowAsJavaScriptException();
		
		delete[] pos;
		delete[] size;
		return info.Env().Undefined();
	}
	}
	
	delete[] pos;
	delete[] size;
	return info.Env().Undefined();
}

Napi::Value Variable::WriteSlice(const Napi::CallbackInfo &info) {
	
	if (info.Length() != static_cast<size_t>(2 * this->ndims + 1)) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	if (!info[2 * this->ndims].IsTypedArray()) {
		Napi::TypeError::New(info.Env(), "Expecting a typed array").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	size_t *pos = new size_t[this->ndims];
	size_t *size = new size_t[this->ndims];
	size_t total_size = 1;
	for (int i = 0; i < this->ndims; i++) {
		pos[i] = info[2 * i].As<Napi::Number>().Int32Value();
		size_t s = info[2 * i + 1].As<Napi::Number>().Int32Value();
		size[i] = s;
		total_size *= s;
	}

	Napi::TypedArray val;

	switch (this->type) {
	case NC_BYTE:
	case NC_CHAR:
		val = info[2 * this->ndims].As<Napi::Int8Array>();
		break;
	case NC_SHORT:
		val = info[2 * this->ndims].As<Napi::Int16Array>();
		break;
	case NC_INT:
		val = info[2 * this->ndims].As<Napi::Int32Array>();
		break;
	case NC_FLOAT:
		val = info[2 * this->ndims].As<Napi::Float32Array>();
		break;
	case NC_DOUBLE:
		val = info[2 * this->ndims].As<Napi::Float64Array>();
		break;
	case NC_UBYTE:
		val = info[2 * this->ndims].As<Napi::Uint8Array>();
		break;
	case NC_USHORT:
		val = info[2 * this->ndims].As<Napi::Uint16Array>();
		break;
	case NC_UINT:
		val = info[2 * this->ndims].As<Napi::Uint32Array>();
		break;
#if NODE_MAJOR_VERSION >= 10
	case NC_UINT64:
		val = info[3 * this->ndims].As<Napi::BigUint64Array>();
		break;
	case NC_INT64:
		val = info[3 * this->ndims].As<Napi::BigInt64Array>();
		break;
#endif
	default:
		Napi::TypeError::New(info.Env(), "Variable type not supported yet").ThrowAsJavaScriptException();
		delete[] pos;
		delete[] size;
		return info.Env().Undefined();

	}
	if (val.ElementLength() != total_size) {
		Napi::TypeError::New(info.Env(), "Wrong size of array").ThrowAsJavaScriptException();
		delete[] pos;
		delete[] size;
		return info.Env().Undefined();
	}
	NC_CALL(nc_put_vara(this->parent_id, this->id, pos, size, val.ArrayBuffer().Data()));
	delete[] pos;
	delete[] size;
	return info.Env().Undefined();
}

Napi::Value Variable::WriteStridedSlice(const Napi::CallbackInfo &info) {
	if (info.Length() != static_cast<size_t>(3 * this->ndims + 1)) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	if (!info[3 * this->ndims].IsTypedArray()) {
		Napi::TypeError::New(info.Env(), "Expecting a typed array").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	size_t *pos = new size_t[this->ndims];
	size_t *size = new size_t[this->ndims];
	ptrdiff_t *stride = new ptrdiff_t[this->ndims];
	size_t total_size = 1;
	for (int i = 0; i < this->ndims; i++) {
		pos[i] = info[3 * i].As<Napi::Number>().Int32Value();
		size_t s = info[3 * i + 1].As<Napi::Number>().Int32Value();
		size[i] = s;
		total_size *= s;
		stride[i] = info[3 * i + 2].As<Napi::Number>().Int32Value();
	}

	Napi::TypedArray val;
	switch (this->type) {
	case NC_BYTE:
	case NC_CHAR:
		val = info[3 * this->ndims].As<Napi::Int8Array>();
		break;
	case NC_SHORT:
		val = info[3 * this->ndims].As<Napi::Int16Array>();
		break;
	case NC_INT:
		val = info[3 * this->ndims].As<Napi::Int32Array>();
		break;
	case NC_FLOAT:
		val = info[3 * this->ndims].As<Napi::Float32Array>();
		break;
	case NC_DOUBLE:
		val = info[3 * this->ndims].As<Napi::Float64Array>();
		break;
	case NC_UBYTE:
		val = info[3 * this->ndims].As<Napi::Uint8Array>();
		break;
	case NC_USHORT:
		val = info[3 * this->ndims].As<Napi::Uint16Array>();
		break;
	case NC_UINT:
		val = info[3 * this->ndims].As<Napi::Uint32Array>();
		break;
#if NODE_MAJOR_VERSION >= 10
	case NC_UINT64:
		val = info[3 * this->ndims].As<Napi::BigUint64Array>();
		break;
	case NC_INT64:
		val = info[3 * this->ndims].As<Napi::BigInt64Array>();
		break;
#endif
	default:
		Napi::TypeError::New(info.Env(), "Variable type not supported yet").ThrowAsJavaScriptException();
		delete[] pos;
		delete[] size;
		delete[] stride;
		return info.Env().Undefined();
	}
	if (val.ElementLength() != total_size) {
		Napi::TypeError::New(info.Env(), "Wrong size of array").ThrowAsJavaScriptException();
		delete[] pos;
		delete[] size;
		delete[] stride;
		return info.Env().Undefined();
		
	}
	NC_CALL(nc_put_vars(this->parent_id, this->id, pos, size, stride, val.ArrayBuffer().Data()));
	
	delete[] pos;
	delete[] size;
	delete[] stride;
	return info.Env().Undefined();
}

Napi::Value Variable::Read(const Napi::CallbackInfo &info) {
	
	
	if (info.Length() != static_cast<size_t>(this->ndims)) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}
	
	size_t *pos = new size_t[this->ndims];
	size_t *size = new size_t[this->ndims];
	for (int i = 0; i < this->ndims; i++) {
		pos[i] = info[i].As<Napi::Number>().Int64Value();
		size[i] = 1;
	}
	Napi::ArrayBuffer buffer;
	Napi::Value result;


	switch (this->type) {
	case NC_BYTE: {
		int8_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_CHAR: {
		char v[2];
		v[1] = 0;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::String::New(info.Env(), v);
	} break;
	case NC_SHORT: {
		int16_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_INT: {
		int32_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_FLOAT: {
		float v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_DOUBLE: {
		double v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_UBYTE: {
		uint8_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_USHORT: {
		uint16_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_UINT: {
		uint32_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
#if NODE_MAJOR_VERSION >= 10
	case NC_UINT64:
		uint64_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::BigInt::New(info.Env(), v);
		break;
	case NC_INT64:
		int64_t v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::BigInt::New(info.Env(), v);
		break;
#endif
	case NC_STRING: {
		std::string v;
		NC_CALL(nc_get_vara(this->parent_id, this->id, pos, size, &v));
		result = Napi::String::New(info.Env(), v);
	} break;
	default: {
		Napi::TypeError::New(info.Env(), "Variable type not supported yet").ThrowAsJavaScriptException();
		
		delete[] pos;
		delete[] size;
		return info.Env().Undefined();
	}
	}
	 
	delete[] pos;
	delete[] size;

	return result;
}

Napi::Value Variable::ReadSlice(const Napi::CallbackInfo &info) {
	if (info.Length() != static_cast<size_t>(2 * this->ndims)) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}

#if NODE_MAJOR_VERSION >= 10
	if (this->type < NC_BYTE || this->type > NC_INT64) {
#else
	if (this->type < NC_BYTE || this->type > NC_UINT) {
#endif		
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
#if NODE_MAJOR_VERSION >= 10
	case NC_UINT64:
		auto array = Napi::Uint64Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
		break;
	case NC_INT64:
		auto array = Napi::Int64Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
		break;
#endif
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

#if NODE_MAJOR_VERSION >= 10
	if (this->type < NC_BYTE || this->type > NC_INT64) {
#else
	if (this->type < NC_BYTE || this->type > NC_UINT) {
#endif		
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
#if NODE_MAJOR_VERSION >= 10
	case NC_UINT64:
		auto array = Napi::Uint64Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
		break;
	case NC_INT64:
		auto array = Napi::Int64Array::New(info.Env(), total_size);
		buffer = array.ArrayBuffer();
		result = array;
		break;
#endif
	}

	NC_CALL(nc_get_vars(this->parent_id, this->id, pos, size, stride, buffer.Data()));

	delete[] pos;
	delete[] size;
	delete[] stride;
	return result;
}

Napi::Value Variable::AddAttribute(const Napi::CallbackInfo &info) {
	
	if (info.Length() < 3) {
		Napi::Error::New(info.Env(), "Wrong number ofarguments").ThrowAsJavaScriptException(); 
		return info.Env().Undefined();
	}
	std::string type_str = info[1].As<Napi::String>().ToString();
	std::string name_ = info[0].As<Napi::String>().ToString();
	int type = get_type(type_str);
	if (type == NC2_ERR) {
		Napi::Error::New(info.Env(), "Unknown variable type").ThrowAsJavaScriptException(); 
		return info.Env().Undefined();
	}

	Napi::Object object = Attribute::Build(info.Env(), name_, this->id, this->parent_id, type);
	
	Attribute *attribute =  Napi::ObjectWrap<Attribute>::Unwrap(object);
	attribute->SetValue(info, info[2]);

	return attribute->GetValue(info);
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
	char name_[NC_MAX_NAME + 1];
	NC_CALL(nc_inq_varname(this->parent_id, this->id, name_));
	name = std::string(name_);
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
	int v;
	NC_CALL(nc_inq_var_fletcher32(this->parent_id, this->id, &v));
	
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
	
	return Napi::String::New(info.Env(), res);
}

void Variable::SetChecksumMode(const Napi::CallbackInfo &info, const Napi::Value &value) {
	std::string arg = value.ToString().Utf8Value();
	int v;
	if (arg == "none") {
		v = NC_NOCHECKSUM;
	} else if (arg == "fletcher32") {
		v = NC_FLETCHER32;
	} else {
		Napi::Error::New(info.Env(), "Unknown value").ThrowAsJavaScriptException();    
		return;
	}
	NC_CALL_VOID(nc_def_var_fletcher32(this->parent_id, this->id, v));
}

Napi::Value Variable::GetChunkMode(const Napi::CallbackInfo &info) {
	int v;
	NC_CALL(nc_inq_var_chunking(this->parent_id, this->id, &v, NULL));
	
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

	return Napi::String::New(info.Env(), res);
}

void Variable::SetChunkMode(const Napi::CallbackInfo &info, const Napi::Value &value) {
	std::string arg = value.ToString().Utf8Value();
	int v;
	if (arg == "contiguous") {
		v = NC_CONTIGUOUS;
	} else if (arg == "chunked") {
		v = NC_CHUNKED;
	} else {
		Napi::Error::New(info.Env(), "Unknown value").ThrowAsJavaScriptException();  
		return;
	}

	int len;
	NC_CALL_VOID(nc_inq_varndims(this->parent_id, this->id, &len));
	
	size_t *sizes = new size_t[len];
	NC_CALL_VOID(nc_inq_var_chunking(this->parent_id, this->id, NULL, sizes));
	NC_CALL_VOID(nc_def_var_chunking(this->parent_id, this->id, v, sizes));
	
	delete[] sizes;
}

Napi::Value Variable::GetChunkSizes(const Napi::CallbackInfo &info) {
	size_t *sizes = new size_t[this->ndims];
	
	NC_CALL(nc_inq_var_chunking(this->parent_id, this->id, NULL, sizes));
	Napi::Array result = Napi::Array::New(info.Env(), this->ndims);
	for (int i = 0; i < this->ndims; i++) {
		result[i] = Napi::Number::New(info.Env(), sizes[i]);
	}
	
	delete[] sizes;
	return result;
}

void Variable::SetChunkSizes(const Napi::CallbackInfo &info, const Napi::Value &value) {
	if (!value.IsArray()) {
		Napi::Error::New(info.Env(), "Expecting an array").ThrowAsJavaScriptException();  
		return;
	}
	Napi::Uint32Array array = value.As<Napi::Uint32Array>();
	if (array.ElementLength() != static_cast<size_t>(this->ndims)) {
		Napi::Error::New(info.Env(), "Wrong array size").ThrowAsJavaScriptException(); 
		return;
	}
	int v;
	NC_CALL_VOID(nc_inq_var_chunking(this->parent_id, this->id, &v, NULL));
	
	size_t *sizes = new size_t[this->ndims];
	for (int i = 0; i < this->ndims; i++) {
		sizes[i] = array[i];
	}

	NC_CALL_VOID(nc_def_var_chunking(this->parent_id, this->id, v, sizes));
	delete[] sizes;
}

Napi::Value Variable::GetFillMode(const Napi::CallbackInfo &info) {
	int v;
	NC_CALL(nc_inq_var_fill(this->parent_id, this->id, &v, NULL));
	
	return Napi::Boolean::New(info.Env(), v==1);
}

void Variable::SetFillMode(const Napi::CallbackInfo &info, const Napi::Value &value) {
	
	if (!value.IsBoolean()) {
		Napi::Error::New(info.Env(), "Expecting a boolean").ThrowAsJavaScriptException();
		return;
	}
	int v = value.As<Napi::Boolean>();
	// if (obj->type < NC_BYTE || obj->type > NC_UINT) {
	// 	isolate->ThrowException(v8::Exception::TypeError(
	// 		v8::String::NewFromUtf8(isolate, "Variable type not supported yet",
	// 								v8::NewStringType::kNormal)
	// 			.ToLocalChecked()));
	// 	return;
	// }
	uint8_t *_value = new uint8_t[type_sizes[this->type]];
	NC_CALL_VOID(nc_inq_var_fill(this->parent_id, this->id, NULL, _value));
	
	NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, v, _value));
	delete[] _value;
}

Napi::Value Variable::GetFillValue(const Napi::CallbackInfo &info) {
	Napi::Value result;
	switch (this->type) {
	case NC_BYTE: {
		int8_t v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_CHAR: {
		char v[2];
		v[1] = 0;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::String::New(info.Env(), v); 
	} break;
	case NC_SHORT: { 
		int16_t v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_INT: {
		int32_t v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_FLOAT: {
		float v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_DOUBLE: {
		double v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_UBYTE: {
		uint8_t v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_USHORT: {
		uint16_t v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	case NC_UINT: {
		uint32_t v;
		NC_CALL(nc_inq_var_fill(this->parent_id, this->id, NULL, &v));
		result = Napi::Number::New(info.Env(), v);
	} break;
	default:
		Napi::Error::New(info.Env(), "Variable type not supported yet").ThrowAsJavaScriptException(); 
		return info.Env().Undefined(); 
	}
		
	return result;
}

void Variable::SetFillValue(const Napi::CallbackInfo &info, const Napi::Value &value) {
	int mode;
	NC_CALL_VOID(nc_inq_var_fill(this->parent_id, this->id, &mode, NULL));
	
	switch (this->type) {
	case NC_BYTE:
	case NC_CHAR: {
		int8_t v = value.ToNumber().Int32Value();
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v));
	} break;
	case NC_SHORT: {
		int16_t v = value.ToNumber().Int32Value();
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v));
	} break;
	case NC_INT: {
		int32_t v = value.ToNumber().Int32Value();
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v));
	} break;
	case NC_FLOAT: {
		float v = value.ToNumber().FloatValue();
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v));
	} break;
	case NC_DOUBLE: { 
		double v = value.ToNumber().DoubleValue();
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v)); 
	} break;
	case NC_UBYTE: { 
		uint8_t v = value.ToNumber().Uint32Value();
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v)); 
	} break;
	case NC_USHORT: {
		uint16_t v = value.ToNumber().Uint32Value();
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v)); 
	} break; 
	case NC_UINT: { 
		uint32_t v = value.ToNumber().Uint32Value(); 
		NC_CALL_VOID(nc_def_var_fill(this->parent_id, this->id, mode, &v)); 
	} break; 
	default:
		Napi::Error::New(info.Env(), "Variable type not supported yet").ThrowAsJavaScriptException();
		return;
	}
}

Napi::Value Variable::GetCompressionShuffle(const Napi::CallbackInfo &info) {
	int v;
	NC_CALL(nc_inq_var_deflate(this->parent_id, this->id, &v, NULL, NULL));
		
	return Napi::Boolean::New(info.Env(), v == 1);
}

void Variable::SetCompressionShuffle(const Napi::CallbackInfo &info, const Napi::Value &value) {
	
	if (!value.IsBoolean()) {
		Napi::Error::New(info.Env(), "Expecting a boolean").ThrowAsJavaScriptException();
		return;
	}
	int v = value.ToBoolean() ? 1 : 0;
	int v1, v2;
	NC_CALL_VOID(nc_inq_var_deflate(this->parent_id, this->id, NULL, &v1, &v2));
	NC_CALL_VOID(nc_def_var_deflate(this->parent_id, this->id, v, v1, v2));
}

Napi::Value Variable::GetCompressionDeflate(const Napi::CallbackInfo &info) {
	int v;
	NC_CALL(nc_inq_var_deflate(this->parent_id, this->id, NULL, &v, NULL));
	return Napi::Boolean::New(info.Env(), v == 1);
}

void Variable::SetCompressionDeflate(const Napi::CallbackInfo &info, const Napi::Value &value) {
	
	if (!value.IsBoolean()) {
		Napi::Error::New(info.Env(), "Expecting a boolean").ThrowAsJavaScriptException();
		return;
	}

	int v = value.ToBoolean() ? 1 : 0;

	int v1, v2;
	NC_CALL_VOID(nc_inq_var_deflate(this->parent_id, this->id, &v1, NULL, &v2));
	
	NC_CALL_VOID(nc_def_var_deflate(this->parent_id, this->id, v1, v, v2));
}

Napi::Value Variable::GetCompressionLevel(const Napi::CallbackInfo &info) {
	int v;
	NC_CALL(nc_inq_var_deflate(this->parent_id, this->id, NULL, NULL, &v));
	
	return Napi::Number::New(info.Env(), v);
}

void Variable::SetCompressionLevel(const Napi::CallbackInfo &info, const Napi::Value &value) {
	if (!value.IsNumber()) {
		Napi::Error::New(info.Env(), "Expecting a non-negative integer").ThrowAsJavaScriptException();
		return;
	}
	int v = value.ToNumber().Uint32Value();
	int v1, v2;
	NC_CALL_VOID(nc_inq_var_deflate(this->parent_id, this->id, &v1, &v2, NULL));
	
	NC_CALL_VOID(nc_def_var_deflate(this->parent_id, this->id, v1, v2, v));
	
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
