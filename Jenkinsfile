def checkoutGit() {
  def url = 'https://github.com/c-toolbox/C-Troll';
  def branch = env.BRANCH_NAME

  if (isUnix()) {
    sh "git clone --recursive --depth 1 ${url} --branch ${branch} --single-branch ."
  }
  else {
    bat "git clone --recursive --depth 1 ${url} --branch ${branch} --single-branch ."
  }
}

def createDirectory(dir) {
  cmake([installation: 'InSearchPath', arguments: "-E make_directory ${dir}"])
}

parallel tools: {
  node('tools') {
    stage('tools/scm') {
      deleteDir();
      checkoutGit();
    }
    stage('tools/cppcheck/create') {
      createDirectory('build');
      sh(
        script: 'cppcheck --enable=all --xml --xml-version=2 -i src/* 2> build/cppcheck.xml',
        label: 'Run CPPCheck'
      )
      recordIssues(
       id: 'tools-cppcheck',
       tool: cppCheck(pattern: 'build/cppcheck.xml')
      )
    }
    cleanWs()
  } // node('tools')
},
linux_gcc_make: { // linux-gcc/build(make)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux-gcc') {
      stage('linux-gcc-make/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-gcc-make/build') {
        cmakeBuild([
          buildDir: 'build-make',
          generator: 'Unix Makefiles',
          installation: "InSearchPath",
          steps: [[ args: "-- -j6", withCmake: true ]]
        ])
        recordIssues(
          id: 'linux-gcc-make',
          tool: gcc()
        )
      }
      cleanWs()
    } // node('linux' && 'gcc')
  }
},
linux_gcc_ninja: { // linux-gcc/build(ninja)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux-gcc') {
      stage('linux-gcc-ninja/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-gcc-ninja/build') {
        cmakeBuild([
          buildDir: 'build-ninja',
          generator: 'Ninja',
          installation: "InSearchPath",
          steps: [[ args: "-- -j6", withCmake: true ]]
        ])
      }
      cleanWs()
    } // node('linux' && 'gcc')
  }
},
linux_clang_make: { // linux-clang/build(make)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux-clang') {
      stage('linux-clang-make/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-clang-make/build') {
        cmakeBuild([
          buildDir: 'build-make',
          generator: 'Unix Makefiles',
          installation: "InSearchPath",
          steps: [[ args: "-- -j6", withCmake: true ]]
        ])
        recordIssues(
          id: 'linux-clang-make',
          tool: clang()
        )
      }
      cleanWs()
    } // node('linux' && 'clang')
  }
},
linux_clang_ninja: { // linux-clang/build(ninja)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux-clang') {
      stage('linux-clang-ninja/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-clang-ninja/build') {
        cmakeBuild([
          buildDir: 'build-ninja',
          generator: 'Ninja',
          installation: "InSearchPath",
          steps: [[ args: "-- -j6", withCmake: true ]]
        ])
      }
      cleanWs()
    } // node('linux' && 'clang')
  }
},
windows_msvc: { // windows/build(msvc)
  if (env.USE_BUILD_OS_WINDOWS == 'true') {
    node('windows') {
      stage('windows-msvc/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('windows-msvc/build') {
        cmakeBuild([
          buildDir: 'build-msvc',
          generator: 'Visual Studio 17 2022',
          installation: "InSearchPath",
          steps: [[ args: "-- /nologo /m:6", withCmake: true ]]
        ])
        recordIssues(
          id: 'windows-msvc',
          tool: msBuild()
        )
      }
      cleanWs()
    } // node('windows')
  }
},
windows_ninja: { // windows/build(msvc)
  if (env.USE_BUILD_OS_WINDOWS == 'true') {
    node('windows') {
      stage('windows-ninja/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('windows-ninja/build') {
        bat(
          script: """
          call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64
          if not exist build-ninja mkdir build-ninja
          cd build-ninja
          cmake -G Ninja ..
          cmake --build . -- -j 6 all
          """,
          label: 'Generate build-scripts with cmake and execute them'
        )
      }
      cleanWs()
    } // node('windows')
  }
},
macos_make: { // macos/build(make)
  if (env.USE_BUILD_OS_MACOS == 'true') {
    node('macos') {
      stage('macos-make/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('macos-make/build') {
        cmakeBuild([
          buildDir: 'build-make',
          generator: 'Unix Makefiles',
          installation: "InSearchPath",
          steps: [[ args: "-- -j6", withCmake: true ]]
        ])
        // For some reason this raises an error
        // ID clang is already used by another action: io.jenkins.plugins.analysis.core.model.ResultAction for Clang (LLVM based)
        // even though we give it a unique ID
        // recordIssues(
        //   id: 'macos-clang',
        //   tool: clang()
        // )
      }
      cleanWs()
    } // node('macos')
  }
},
macos_xcode: { // macos_xcode
  if (env.USE_BUILD_OS_MACOS == 'true') {
    node('macos') {
      stage('macos-xcode/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('macos-xcode/build') {
        cmakeBuild([
          buildDir: 'build-xcode',
          generator: 'Xcode',
          installation: "InSearchPath",
          steps: [[ args: "-- -quiet -parallelizeTargets -jobs 6", withCmake: true ]]
        ])
      }
      cleanWs()
    } // node('macos')
  }
}
