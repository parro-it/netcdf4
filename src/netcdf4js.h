#ifndef NETCDF4JS_H
#define NETCDF4JS_H

#include <napi.h>
#include <netcdf.h>
#include <string>
#include <memory>

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
	if (type_str == "byte" || type_str == "i1" || type_str == "b" || type_str == "B") {
		return NC_BYTE;
	} else if (type_str == "char") {
		return NC_CHAR;
	} else if (type_str == "short" || type_str == "i2" || type_str == "h" || type_str == "s") {
		return NC_SHORT;
	} else if (type_str == "int" || type_str == "i4" || type_str == "i" || type_str == "l") {
		return NC_INT;
	} else if (type_str == "float" || type_str == "f4" || type_str == "f") {
		return NC_FLOAT;
	} else if (type_str == "double" || type_str == "f8" || type_str == "d") {
		return NC_DOUBLE;
	} else if (type_str == "ubyte" || type_str == "u1") {
		return NC_UBYTE;
	} else if (type_str == "ushort" || type_str == "u2") {
		return NC_USHORT;
	} else if (type_str == "uint" || type_str == "u4") {
		return NC_UINT;
	} else if (type_str == "uint64" || type_str == "u8") {
		return NC_UINT64;
	} else if (type_str == "int64" || type_str == "i8") {
		return NC_INT64;
	} else if (type_str == "string" || type_str == "S1") {
		return NC_STRING;
	} else {
		return NC2_ERR;
	}
}


/**
 * printf like formatting for C++ with std::string
 * Original source: https://stackoverflow.com/a/26221725/11722
 */
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
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
	Napi::Value DataMode(const Napi::CallbackInfo &info);
	Napi::Value GetId(const Napi::CallbackInfo &info);
	Napi::Value GetName(const Napi::CallbackInfo &info);
	Napi::Value IsClosed(const Napi::CallbackInfo &info);
	Napi::Value GetFormat(const Napi::CallbackInfo &info);
	Napi::Value Inspect(const Napi::CallbackInfo &info);

	int id;
	std::string name;
	std::string format;
	bool closed;
};

class Group : public Napi::ObjectWrap<Group> {
  public:
	Group(const Napi::CallbackInfo &info);
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	static Napi::Object Build(Napi::Env env, int id);
	bool get_name(char *name) const;
	// Napi::Value GetName(const Napi::CallbackInfo &info);

  private:
	static Napi::FunctionReference constructor;
	Napi::Value GetId(const Napi::CallbackInfo &info);
	Napi::Value GetVariables(const Napi::CallbackInfo &info);
	Napi::Value GetDimensions(const Napi::CallbackInfo &info);
	Napi::Value GetUnlimited(const Napi::CallbackInfo &info);
	Napi::Value GetAttributes(const Napi::CallbackInfo &info);
	Napi::Value GetSubgroups(const Napi::CallbackInfo &info);
	Napi::Value GetName(const Napi::CallbackInfo &info);
	void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);
	Napi::Value GetFullname(const Napi::CallbackInfo &info);
	Napi::Value AddAttribute(const Napi::CallbackInfo &info);
	Napi::Value AddDimension(const Napi::CallbackInfo &info);
	Napi::Value AddSubgroup(const Napi::CallbackInfo &info);
	Napi::Value AddVariable(const Napi::CallbackInfo &info);
	Napi::Value Inspect(const Napi::CallbackInfo &info);
	int id;
	std::string name;
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
	std::string name;
	size_t length;
};

class Attribute : public Napi::ObjectWrap<Attribute> {
  public:
    static void set_value(const Napi::CallbackInfo &info,int parent_id,int var_id,std::string name,int type,const Napi::Value &value);
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	static Napi::Object Build(Napi::Env env, std::string name, int var_id, int parent_id, int type);
	static Napi::Object Build(Napi::Env env, std::string name, int var_id, int parent_id);
	Attribute(const Napi::CallbackInfo &info);

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
	int id;
	int parent_id;
	nc_type type;
	int ndims;
	std::string name;
};

} // namespace netcdf4js

#endif
