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
                '-Werror',
                '-fexceptions',
                '-std=c++14'
            ],
            "sources": [
                "src/Group.cpp",
                "src/File.cpp",
                "src/Variable.cpp",
                "src/Dimension.cpp",
                "src/Attribute.cpp",
                "src/netcdf4js.cpp",
                "src/netcdf4jstypes.cpp"
            ],
            "target_name": "netcdf4",
            "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
            "target_name": "netcdf4",
            'defines': ['NAPI_CPP_EXCEPTIONS'],
            "conditions": [
                ['OS=="mac"', {
                    'cflags+': ['-fvisibility=hidden'],
                    'xcode_settings': {
                        'WARNING_CFLAGS!': ['-Wall', '-Wextra'],
                        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',  # -fvisibility=hidden
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'CLANG_CXX_LIBRARY': 'libc++',
                        'MACOSX_DEPLOYMENT_TARGET': '10.7',
                        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11']                        
                    }
                }],
                ['OS=="win"', {
                    "variables": {
                        "netcdf_dir%": "<!(echo %NETCDF_DIR%)"
                    },
                    "include_dirs": [
                        "<(netcdf_dir)/include"
                    ],
                    "msvs_settings": {
                        'VCCLCompilerTool': {
                            'WarningLevel': '3',
                            'WarnAsError': 'false',
                            'ExceptionHandling': 1
                        },
                        "VCLinkerTool": {
                            "AdditionalLibraryDirectories": "<(netcdf_dir)/lib"
                        }
                    }
                }]
            ]
        }
    ]
}
