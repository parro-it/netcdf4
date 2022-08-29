#include "netcdf4jstypes.h"

namespace netcdf4js {
    const char *type_names[] = {
        "unknown", // NC_NAT // unknown type
        "byte",	   // NC_BYTE
        "char",	   // NC_CHAR
        "short",   // NC_SHORT
        "int",	   // NC_INT / NC_LONG
        "float",   // NC_FLOAT
        "double",  // NC_DOUBLE
        "ubyte",   // NC_UBYTE
        "ushort",  // NC_USHORT
        "uint",	   // NC_UINT
        "int64",	   // NC_INT64
        "uint64",	   // NC_UINT64
    };
    const char *format_names[] = {
        "unknown", // NC_FORMATX_UNDEFINED
        "netcdf3",	   // NC_FORMATX_NC3
        "hdf5",	   // NC_FORMATX_HDF5
        "hdf4",   // NC_FORMATX_HDF4 / NC_FORMATX_NC4
        "pnetcdf", // NC_FORMATX_PNETCDF
        "dap2",   // NC_FORMATX_DAP2
        "dap4",  // NC_FORMATX_DAP4
        "?",   // 
        "udf0",  // NC_FORMATX_UDF0
        "udf1",	   // NC_FORMATX_UDF1
        "zarr"	   // NC_FORMATX_NCZARR
    };
}
