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
        "uint"	   // NC_UINT
    };
}
