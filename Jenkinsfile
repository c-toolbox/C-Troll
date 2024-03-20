def url = "https://github.com/c-toolbox/C-Troll";
def branch = env.BRANCH_NAME

parallel tools: {
  node("tools") {
    stage("tools/scm") {
      deleteDir();
      checkout scm
      sh(
        script: git submodule update --init,
        label: "Init submodules"
      )
    }
    stage("tools/cppcheck/run") {
      sh(
        script: "cppcheck --platform=win64 --enable=all --quiet --xml --xml-version=2 -I src/common/include -I ext/fmt/include --suppress=*:ext/fmt/include/* -I ext/json/include --suppress=unusedFunction --suppress=missingInclude --suppress=*:ext/json/include/* -I ext/simplecrypt/include -UJSON_ASSERT -UJSON_CATCH_USER -DJSON_DISABLE_ENUM_SERIALIZATION=0 -UJSON_HAS_EXPERIMENTAL_FILESYSTEM -UJSON_HAS_FILESYSTEM -UJSON_HAS_RANGES -UJSON_HAS_THREE_WAY_COMPARISON -UJSON_HEDLEY_ALWAYS_INLINE -UJSON_HEDLEY_VERSION -UJSON_HEDLEY_ARM_VERSION -UJSON_HEDLEY_ARM_VERSION_CHECK -UJSON_HAS_CPP_11 -UJSON_HAS_CPP_14 -UJSON_HAS_CPP_17 -UJSON_HAS_CPP_20 src 2> cppcheck.xml",
        label: "Run CPPCheck"
      )
    }
    stage("tools/cppcheck/record") {
      recordIssues(
        id: "tools-cppcheck",
        tool: cppCheck(pattern: "cppcheck.xml")
      )
    }
    cleanWs()
  } // node('tools')
},
windows_msvc: { // windows/build(msvc)
  if (env.USE_BUILD_OS_WINDOWS == "true") {
    node("windows") {
      stage("windows-msvc/scm") {
        deleteDir();
        checkout scm
        bat(
          script: git submodule update --init,
          label: "Init submodules"
        )
      }
      stage("windows-msvc/build") {
        cmakeBuild([
          buildDir: "build-msvc",
          generator: "Visual Studio 17 2022",
          installation: "InSearchPath",
          steps: [[ args: "-- /nologo /m:6", withCmake: true ]]
        ])
        recordIssues(
          id: "windows-msvc",
          tool: msBuild()
        )
      }
      cleanWs()
    } // node('windows')
  }
}
