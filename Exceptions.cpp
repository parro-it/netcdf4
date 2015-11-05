#include <netcdf.h>

namespace netcdf4js {
    const char* get_error_message(int retval) {
        switch(retval) {
        case NC_NOERR           : return nullptr;
        case NC_EBADID          : return "Not a netcdf id";
        case NC_ENFILE          : return "Too many netcdfs open";
        case NC_EEXIST          : return "netcdf file exists && NC_NOCLOBBER";
        case NC_EINVAL          : return "Invalid Argument";
        case NC_EPERM           : return "Write to read only";
        case NC_ENOTINDEFINE    : return "Operation not allowed in data mode";
        case NC_EINDEFINE       : return "Operation not allowed in define mode";
        case NC_EINVALCOORDS    : return "Index exceeds dimension bound";
        case NC_EMAXDIMS        : return "NC_MAX_DIMS is exceeded";
        case NC_ENAMEINUSE      : return "String match to name in use";
        case NC_ENOTATT         : return "Attribute not found";
        case NC_EMAXATTS        : return "NC_MAX_ATTRS exceeded";
        case NC_EBADTYPE        : return "Not a netcdf data type";
        case NC_EBADDIM         : return "Invalid dimension id or name";
        case NC_EUNLIMPOS       : return "NC_UNLIMITED is in the wrong index";
        case NC_EMAXVARS        : return "NC_MAX_VARS is exceeded";
        case NC_ENOTVAR         : return "Variable is not found";
        case NC_EGLOBAL         : return "Action prohibited on NC_GLOBAL varid";
        case NC_ENOTNC          : return "Not a netcdf file";
        case NC_ESTS            : return "In Fortran, string too short";
        case NC_EMAXNAME        : return "NC_MAX_NAME exceeded";
        case NC_EUNLIMIT        : return "NC_UNLIMITED size already in use";
        case NC_ENORECVARS      : return "nc_rec op when there are no record vars";
        case NC_ECHAR           : return "Attempt to convert between text & numbers";
        case NC_EEDGE           : return "Edge+start exceeds dimension bound";
        case NC_ESTRIDE         : return "Illegal stride";
        case NC_EBADNAME        : return "Attribute or variable name contains illegal characters";
        case NC_ERANGE          : return "Math result not representable";
        case NC_ENOMEM          : return "Memory allocation (malloc) failure";
        case NC_EVARSIZE        : return "One or more variable sizes violate format constraints";
        case NC_EDIMSIZE        : return "Invalid dimension size";
        case NC_ETRUNC          : return "File likely truncated or possibly corrupted";
        case NC_EHDFERR         : return "An error was reported by the HDF5 layer.";
        case NC_ECANTREAD       : return "Cannot Read";
        case NC_ECANTWRITE      : return "Cannott write";
        case NC_ECANTCREATE     : return "Cannot create";
        case NC_EFILEMETA       : return "File  meta";
        case NC_EDIMMETA        : return "dim meta";
        case NC_EATTMETA        : return "att meta";
        case NC_EVARMETA        : return "var meta";
        case NC_ENOCOMPOUND     : return "No compound";
        case NC_EATTEXISTS      : return "Attribute exists";
        case NC_ENOTNC4         : return "Attempting netcdf-4 operation on netcdf-3 file.";
        case NC_ESTRICTNC3      : return "Attempting netcdf-4 operation on strict nc3 netcdf-4 file.";
        case NC_EBADGRPID       : return "Bad group id.";
        case NC_EBADTYPID       : return "Bad type id.";
        case NC_EBADFIELD       : return "Bad field id.";
        case NC_ENOGRP          : return "No netCDF group found";
        case NC_ELATEDEF        : return "Operation to set the deflation, chunking, endianness, fill, compression, or checksum of a NcVar object has been made after a call to getVar or putVar.";
        default                 : return "Unknown error";
        }
    }
}
