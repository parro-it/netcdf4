#include <netcdf.h>
#include <stdio.h>
#include <string>
#include "netcdf4js.h"

namespace netcdf4js {

Napi::FunctionReference Attribute::constructor;

Napi::Object Attribute::Build(Napi::Env env, std::string name, int var_id, int parent_id,
							  int type) {
	return constructor.New({
		Napi::String::New(env, name),
		Napi::Number::New(env, var_id),
		Napi::Number::New(env, parent_id),
		Napi::Number::New(env, type),
	});
}

Napi::Object Attribute::Build(Napi::Env env, std::string name, int var_id, int parent_id) {
	return constructor.New({Napi::String::New(env, name), Napi::Number::New(env, var_id),
							Napi::Number::New(env, parent_id)});
}

Attribute::Attribute(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Attribute>(info) {
	if (info.Length() < 3) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return;
	}

	name = info[0].As<Napi::String>().Utf8Value();
	var_id = info[1].As<Napi::Number>().Int32Value();
	parent_id = info[2].As<Napi::Number>().Int32Value();

	if (info.Length() > 3) {
		type = info[3].As<Napi::Number>().Int32Value();
	} else {
		NC_CALL_VOID(nc_inq_atttype(parent_id, var_id, name.c_str(), &this->type));
	}

	NC_CALL_VOID(nc_inq_attid(parent_id, var_id, name.c_str(), &this->id));
}

Napi::Object Attribute::Init(Napi::Env env, Napi::Object exports) {
	Napi::HandleScope scope(env);

	auto properties = {
		InstanceMethod("inspect", &Attribute::Inspect),
		InstanceMethod("delete", &Attribute::Delete),
		InstanceAccessor<&Attribute::GetValue, &Attribute::SetValue>("value"),
		InstanceAccessor<&Attribute::GetName, &Attribute::SetName>("name"),
	};

	Napi::Function func = DefineClass(env, "Attribute", properties);
	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Attribute", func);
	return exports;
}

Napi::Value Attribute::GetName(const Napi::CallbackInfo &info) {
	return Napi::String::New(info.Env(), name);
}

void Attribute::SetName(const Napi::CallbackInfo &info, const Napi::Value &value) {

	std::string new_name = value.As<Napi::String>().ToString();
	NC_CALL_VOID(
		nc_rename_att(this->parent_id, this->var_id, this->name.c_str(), new_name.c_str()));
	this->name = new_name;
}

Napi::Value Attribute::GetValue(const Napi::CallbackInfo &info) {

	if ((this->type < NC_BYTE || this->type > NC_UINT64) && this->type != NC_STRING) {
		Napi::TypeError::New(info.Env(), "Variable type not supported yet")
			.ThrowAsJavaScriptException();
		return info.Env().Undefined();
	}

	size_t len;
	NC_CALL(nc_inq_attlen(this->parent_id, this->var_id, this->name.c_str(), &len));

	switch (this->type) {
	case NC_BYTE: {
		int8_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Int8Array::New(info.Env(), sizeof(int8_t),
										Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(int8_t)),
										0, napi_int8_array);
		}
	}
	case NC_SHORT: {
		int16_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Int16Array::New(
				info.Env(), sizeof(int16_t),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(int16_t)), 0, napi_int8_array);
		}
	}
	case NC_INT: {
		int32_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Int32Array::New(
				info.Env(), sizeof(int32_t),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(int32_t)), 0, napi_int8_array);
		}
	}
	case NC_FLOAT: {
		float v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Float32Array::New(
				info.Env(), sizeof(float),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(float)), 0, napi_int8_array);
		}
	}
	case NC_DOUBLE: {
		double v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Float64Array::New(
				info.Env(), sizeof(double),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(double)), 0, napi_int8_array);
		}
	}
	case NC_UBYTE: {
		uint8_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Uint8Array::New(
				info.Env(), sizeof(uint8_t),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(uint8_t)), 0, napi_int8_array);
		}
	}
	case NC_USHORT: {
		uint16_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Uint16Array::New(
				info.Env(), sizeof(uint16_t),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(uint16_t)), 0, napi_int8_array);
		}
	}
	case NC_UINT: {
		uint32_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::Number::New(info.Env(), v[0]);
		} else {
			return Napi::Uint32Array::New(
				info.Env(), sizeof(uint32_t),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(uint32_t)), 0, napi_int8_array);
		}
	}
	case NC_UINT64: {
		uint64_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::BigInt::New(info.Env(), v[0]);
		} else {
			return Napi::BigUint64Array::New(
				info.Env(), sizeof(uint64_t),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(uint64_t)), 0, napi_int8_array);
		}
	}
	case NC_INT64: {
		int64_t v[len];
		NC_CALL(nc_get_att(this->parent_id, this->var_id, this->name.c_str(), v));
		if (len == 1) {
			return Napi::BigInt::New(info.Env(), v[0]);
		} else {
			return Napi::BigInt64Array::New(
				info.Env(), sizeof(int64_t),
				Napi::ArrayBuffer::New(info.Env(), v, len * sizeof(int64_t)), 0, napi_int8_array);
		}
	}
	case NC_CHAR:
	case NC_STRING: {
		char v[len + 1];
		v[len] = 0;
		NC_CALL(nc_get_att_text(this->parent_id, this->var_id, this->name.c_str(), v));
		return Napi::String::New(info.Env(), v);
	}
	}

	return info.Env().Undefined();
}

void Attribute::SetValue(const Napi::CallbackInfo &info, const Napi::Value &value) {
	if ((this->type < NC_BYTE || this->type > NC_UINT64) && this->type != NC_STRING) {
		Napi::TypeError::New(info.Env(), "Variable type not supported yet")
			.ThrowAsJavaScriptException();
		return;
	}

	switch (this->type) {
	case NC_BYTE: {
		if (value.IsNumber()) {
			int8_t v = value.As<Napi::Number>().Int32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			int8_t v = value.As<Napi::BigInt>().Int64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Int8Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_SHORT: {
		if (value.IsNumber()) {
			int16_t v = value.As<Napi::Number>().Int32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			int16_t v = value.As<Napi::BigInt>().Int64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Int16Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_INT: {
		if (value.IsNumber()) {
			int32_t v = value.As<Napi::Number>().Int32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			int32_t v = value.As<Napi::BigInt>().Int64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Int32Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_FLOAT: {
		if (value.IsNumber()) {
			float v = value.As<Napi::Number>().DoubleValue();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			float v = value.As<Napi::BigInt>().Int64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Float32Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_DOUBLE: {
		if (value.IsNumber()) {
			double v = value.As<Napi::Number>().DoubleValue();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			double v = value.As<Napi::BigInt>().Int64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Float64Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_UBYTE: {
		if (value.IsNumber()) {
			uint8_t v = value.As<Napi::Number>().Uint32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			uint8_t v = value.As<Napi::BigInt>().Uint64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Uint8Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_USHORT: {
		if (value.IsNumber()) {
			uint16_t v = value.As<Napi::Number>().Uint32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			uint16_t v = value.As<Napi::BigInt>().Uint64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Uint16Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_UINT: {
		if (value.IsNumber()) {
			uint32_t v = value.As<Napi::Number>().Uint32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			uint32_t v = value.As<Napi::BigInt>().Uint64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::Uint32Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_UINT64: {
		if (value.IsNumber()) {
			uint64_t v = value.As<Napi::Number>().Uint32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			uint64_t v = value.As<Napi::BigInt>().Uint64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::BigUint64Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_INT64: {
		if (value.IsNumber()) {
			int64_t v = value.As<Napi::Number>().Uint32Value();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else if (value.IsBigInt()) {
			int64_t v = value.As<Napi::BigInt>().Uint64Value(nullptr);
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, 1, &v));
		} else {
			auto array = value.As<Napi::BigInt64Array>();
			void *v = array.ArrayBuffer().Data();
			size_t l = array.ElementLength();
			NC_CALL_VOID(nc_put_att(parent_id, var_id, name.c_str(), this->type, l, &v));
		}
	} break;
	case NC_CHAR:
	case NC_STRING: {
		std::string v = value.As<Napi::String>().ToString();
		nc_put_att_text(parent_id, var_id, name.c_str(), v.length(), v.c_str());
	} break;
	}
}

Napi::Value Attribute::Delete(const Napi::CallbackInfo &info) {
	/*
	Attribute *obj = node::ObjectWrap::Unwrap<Attribute>(args.Holder());
	int retval = nc_del_att(obj->parent_id, obj->var_id, obj->name.c_str());
	if (retval != NC_NOERR) {
		throw_netcdf_error(args.GetIsolate(), retval);
	}
	*/
	return info.Env().Undefined();
}

Napi::Value Attribute::Inspect(const Napi::CallbackInfo &info) {
	return Napi::String::New(info.Env(), "[object Attribute]");
}
} // namespace netcdf4js
