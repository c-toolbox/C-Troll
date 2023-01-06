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
        script: 'cppcheck --enable=all --xml --xml-version=2  -I src/common/include -I ext/fmt/include -I ext/json/include -I ext/simplecrypt/include -i src/* 2> build/cppcheck.xml',
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
}

