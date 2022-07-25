{
  "targets": [
    {
      "libraries": [
        "-lnetcdf",
        "-lhdf5"
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
