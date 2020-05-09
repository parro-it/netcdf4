{
    "targets": [
        {
            "libraries": [
                "-lnetcdf"
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
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions'],
            'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'CLANG_CXX_LIBRARY': 'libc++',
                'MACOSX_DEPLOYMENT_TARGET': '10.7',
            },
            'msvs_settings': {
                'VCCLCompilerTool': {'ExceptionHandling': 1},
            },
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
