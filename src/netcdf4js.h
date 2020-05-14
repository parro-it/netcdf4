#ifndef NETCDF4JS_H
#define NETCDF4JS_H

#include <napi.h>
#include <netcdf.h>
#include <string>

#define NC_CALL(FN)                                                                                \
	do {                                                                                           \
		int retval = FN;                                                                           \
		if (retval != NC_NOERR) {                                                                  \
			Napi::Error::New(info.Env(), nc_strerror(retval)).ThrowAsJavaScriptException();        \
			return info.Env().Undefined();                                                         \
		}                                                                                          \
	} while (false);

#define NC_CALL_VOID(FN)                                                                           \
	do {                                                                                           \
		int retval = FN;                                                                           \
		if (retval != NC_NOERR) {                                                                  \
			Napi::Error::New(info.Env(), nc_strerror(retval)).ThrowAsJavaScriptException();        \
			return;                                                                                \
		}                                                                                          \
	} while (false);

#define NC_CALL_ENV(ENV, FN)                                                                       \
	do {                                                                                           \
		int retval = FN;                                                                           \
		if (retval != NC_NOERR) {                                                                  \
			Napi::Error::New(ENV, nc_strerror(retval)).ThrowAsJavaScriptException();               \
			return;                                                                                \
		}                                                                                          \
	} while (false);

namespace netcdf4js {
/*
inline void throw_netcdf_error(v8::Isolate *isolate, int retval) {
	isolate->ThrowException(v8::Exception::TypeError(
		v8::String::NewFromUtf8(isolate, nc_strerror(retval), v8::NewStringType::kNormal)
			.ToLocalChecked()));
}
*/
inline int get_type(const std::string &type_str) {
	if (type_str == "byte") {
		return NC_BYTE;
	} else if (type_str == "char") {
		return NC_CHAR;
	} else if (type_str == "short") {
		return NC_SHORT;
	} else if (type_str == "int") {
		return NC_INT;
	} else if (type_str == "float") {
		return NC_FLOAT;
	} else if (type_str == "double") {
		return NC_DOUBLE;
	} else if (type_str == "ubyte") {
		return NC_UBYTE;
	} else if (type_str == "ushort") {
		return NC_USHORT;
	} else if (type_str == "uint") {
		return NC_UINT;
	} else if (type_str == "string") {
		return NC_STRING;
	} else {
		return NC2_ERR;
	}
}

class File : public Napi::ObjectWrap<File> {
  public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	explicit File(const Napi::CallbackInfo &info);
	~File();

  private:
	static Napi::FunctionReference constructor;

	bool open(const char *filename, const int &mode, const int &format);
	// Napi::Value New(const Napi::CallbackInfo &info);
	Napi::Value Close(const Napi::CallbackInfo &info);
	Napi::Value Sync(const Napi::CallbackInfo &info);
	Napi::Value Inspect(const Napi::CallbackInfo &info);

	int id;
	bool closed;
};

class Group : public Napi::ObjectWrap<Group> {
  public:
	Group(const Napi::CallbackInfo &info);
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	static Napi::Object Build(Napi::Env env, int id);
	bool get_name(char *name) const;

  private:
	static Napi::FunctionReference constructor;
	Napi::Value GetId(const Napi::CallbackInfo &info);
	Napi::Value GetVariables(const Napi::CallbackInfo &info);
	Napi::Value GetDimensions(const Napi::CallbackInfo &info);
	Napi::Value GetUnlimited(const Napi::CallbackInfo &info);
	Napi::Value GetAttributes(const Napi::CallbackInfo &info);
	Napi::Value GetSubgroups(const Napi::CallbackInfo &info);
	Napi::Value GetName(const Napi::CallbackInfo &info);
	Napi::Value GetFullname(const Napi::CallbackInfo &info);
	Napi::Value AddAttribute(const Napi::CallbackInfo &info);
	Napi::Value AddDimension(const Napi::CallbackInfo &info);
	Napi::Value AddSubgroup(const Napi::CallbackInfo &info);
	Napi::Value AddVariable(const Napi::CallbackInfo &info);
	Napi::Value Inspect(const Napi::CallbackInfo &info);
	int id;
};

class Dimension : public Napi::ObjectWrap<Dimension> {
  public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	static Napi::Object Build(Napi::Env env, int id, int parent_id);
	Dimension(const Napi::CallbackInfo &info);
	bool get_name(char *name) const;

  private:
	static Napi::FunctionReference constructor;
	Napi::Value GetId(const Napi::CallbackInfo &info);
	Napi::Value GetLength(const Napi::CallbackInfo &info);
	Napi::Value GetName(const Napi::CallbackInfo &info);
	void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value Inspect(const Napi::CallbackInfo &info);
	int id;
	int parent_id;
};

class Attribute : public Napi::ObjectWrap<Attribute> {
  public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	static Napi::Object Build(Napi::Env env, std::string name, int var_id, int parent_id, int type);
	static Napi::Object Build(Napi::Env env, std::string name, int var_id, int parent_id);
	Attribute(const Napi::CallbackInfo &info);
	void set_value(Napi::Value &val);

  private:
	static Napi::FunctionReference constructor;
	Napi::Value Delete(const Napi::CallbackInfo &info);
	Napi::Value GetName(const Napi::CallbackInfo &info);
	void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetValue(const Napi::CallbackInfo &info);
	void SetValue(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value Inspect(const Napi::CallbackInfo &info);
	std::string name;
	int id;
	int var_id;
	int parent_id;
	int type;
};

class Variable : public Napi::ObjectWrap<Variable> {
  public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	Variable(const Napi::CallbackInfo &info);
	static Napi::Object Build(Napi::Env env, int id, int parent_id);

  private:
	Napi::Value Read(const Napi::CallbackInfo &info);
	Napi::Value ReadSlice(const Napi::CallbackInfo &info);
	Napi::Value ReadStridedSlice(const Napi::CallbackInfo &info);
	Napi::Value Write(const Napi::CallbackInfo &info);
	Napi::Value WriteSlice(const Napi::CallbackInfo &info);
	Napi::Value WriteStridedSlice(const Napi::CallbackInfo &info);
	Napi::Value AddAttribute(const Napi::CallbackInfo &info);
	Napi::Value GetId(const Napi::CallbackInfo &info);
	Napi::Value GetType(const Napi::CallbackInfo &info);
	Napi::Value GetDimensions(const Napi::CallbackInfo &info);
	Napi::Value GetAttributes(const Napi::CallbackInfo &info);
	Napi::Value GetName(const Napi::CallbackInfo &info);
	void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetEndianness(const Napi::CallbackInfo &info);
	void SetEndianness(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetChecksumMode(const Napi::CallbackInfo &info);
	void SetChecksumMode(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetChunkMode(const Napi::CallbackInfo &info);
	void SetChunkMode(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetChunkSizes(const Napi::CallbackInfo &info);
	void SetChunkSizes(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetFillMode(const Napi::CallbackInfo &info);
	void SetFillMode(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetFillValue(const Napi::CallbackInfo &info);
	void SetFillValue(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetCompressionShuffle(const Napi::CallbackInfo &info);
	void SetCompressionShuffle(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetCompressionDeflate(const Napi::CallbackInfo &info);
	void SetCompressionDeflate(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetCompressionLevel(const Napi::CallbackInfo &info);
	void SetCompressionLevel(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value Inspect(const Napi::CallbackInfo &info);
	static Napi::FunctionReference constructor;

	static const unsigned char type_sizes[];
	static const char *type_names[];
	int id;
	int parent_id;
	nc_type type;
	int ndims;
};

} // namespace netcdf4js

#endif
