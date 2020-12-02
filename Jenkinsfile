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
        script: 'cppcheck --enable=all --xml --xml-version=2 -i ext common core tray 2> build/cppcheck.xml',
        label: 'Run CPPCheck'
      )
      recordIssues(
       id: 'tools-cppcheck',
       tool: cppCheck(pattern: 'build/cppcheck.xml')
      )
    }
    // stage('tools/cloc/create') {
    //   createDirectory('build');
    //   sh 'cloc --by-file --exclude-dir=build,example,ext --xml --out=build/cloc.xml --quiet .';
    // }
    cleanWs()
  } // node('tools')
},
linux_gcc_make: { // linux-gcc/build(make)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux' && 'gcc') {
      stage('linux-gcc/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-gcc/build(make)') {
        cmakeBuild([
          buildDir: 'build-make',
          generator: 'Unix Makefiles',
          installation: "InSearchPath",
          steps: [[ args: "-- -j4", withCmake: true ]]
        ])
        recordIssues(
          id: 'linux-gcc',
          tool: gcc()
        )
      }
      cleanWs()
    } // node('linux' && 'gcc')
  }
},
linux_gcc_ninja: { // linux-gcc/build(ninja)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux' && 'gcc') {
      stage('linux-gcc/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-gcc/build(ninja)') {
        cmakeBuild([
          buildDir: 'build-ninja',
          generator: 'Ninja',
          installation: "InSearchPath",
          steps: [[ args: "-- -j4", withCmake: true ]]
        ])
      }    
      cleanWs()
    } // node('linux' && 'gcc')
  }
},
linux_clang_make: { // linux-clang/build(make)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux' && 'clang') {
      stage('linux-clang/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-clang/build(make)') {
        cmakeBuild([
          buildDir: 'build-make',
          generator: 'Unix Makefiles',
          installation: "InSearchPath",
          steps: [[ args: "-- -j4", withCmake: true ]]
        ])
        recordIssues(
          id: 'linux-clang',
          tool: clang()
        )
      }
      cleanWs()
    } // node('linux' && 'clang')
  }
},
linux_clang_ninja: { // linux-clang/build(ninja)
  if (env.USE_BUILD_OS_LINUX == 'true') {
    node('linux' && 'clang') {
      stage('linux-clang/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('linux-clang/build(ninja)') {
        cmakeBuild([
          buildDir: 'build-ninja',
          generator: 'Ninja',
          installation: "InSearchPath",
          steps: [[ args: "-- -j4", withCmake: true ]]
        ])
      } 
      cleanWs()
    } // node('linux' && 'clang')
  }
},
windows_msvc: { // windows/build(msvc)
  if (env.USE_BUILD_OS_WINDOWS == 'true') {
    node('windows') {
      stage('windows/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('windows/build(msvc)') {
        cmakeBuild([
          buildDir: 'build-msvc',
          generator: 'Visual Studio 16 2019',
          installation: "InSearchPath",
          steps: [[ args: "-- /nologo /m:4", withCmake: true ]]
        ])
        recordIssues(
          id: 'windows-msbuild',
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
      stage('windows/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('windows/build(ninja)') {
        bat(
          script: """
          call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64
          if not exist build-ninja mkdir build-ninja
          cd build-ninja
          cmake -G Ninja ..
          cmake --build . -- -j 4 all
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
      stage('macos/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('macos/build(make)') {
        cmakeBuild([
          buildDir: 'build-make',
          generator: 'Unix Makefiles',
          installation: "InSearchPath",
          steps: [[ args: "-- -j4", withCmake: true ]]
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
macos_ninja: { // macos/build(make)
  if (env.USE_BUILD_OS_MACOS == 'true') {
    node('macos') {
      stage('macos/scm') {
        deleteDir();
        checkoutGit();
      }
      stage('macos/build(xcode)') {
        cmakeBuild([
          buildDir: 'build-xcode',
          generator: 'Xcode',
          installation: "InSearchPath",
          steps: [[ args: "-- -quiet -parallelizeTargets -jobs 4", withCmake: true ]]
        ])
      }
      cleanWs()
    } // node('macos')
  }
}
