{
    "targets": [
        {
            "libraries": [
                "-lnetcdf"
            ],
            'cflags_cc': [
                '-Wall',
                '-Wextra',
                '--pedantic-errors',
                '-Wfloat-equal',
                '-Wuninitialized',
                '-Wunreachable-code',
                '-Wold-style-cast',
                '-Werror'
            ],
            "sources": [
                "src/Group.cpp",
                "src/File.cpp",
                "src/Variable.cpp",
                "src/Dimension.cpp",
                "src/Attribute.cpp",
                "src/netcdf4js.cpp"
            ],
            "target_name": "netcdf4",
            "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
            "target_name": "netcdf4",
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            "conditions": [
                ['OS=="win"', {
                    "variables": {
                        "netcdf_dir%": "<!(echo %NETCDF_DIR%)"
                    },
                    "include_dirs": [
                        "<(netcdf_dir)/include"
                    ],
                    "msvs_settings": {
                        "VCLinkerTool": {
                            "AdditionalLibraryDirectories": "<(netcdf_dir)/lib"
                        }
                    }
                }]
            ]
        }
    ]
}
