{
  "targets": [
    {
      "libraries": [
        "-lnetcdf"
      ],
      "sources": [
        "src/File.cpp",
        "src/Attribute.cpp",
        "src/Group.cpp",
        "src/module.c",
        "src/Variable.cpp",
        "src/Utils.cpp",
        "src/Dimension.cpp",
        "src/napi-utils.c"
      ],
      "cpp": [
        "src/netcdf4js.cpp"
      ],
      "target_name": "netcdf4",
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
