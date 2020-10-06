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

def build() {
  cmake([installation: 'InSearchPath', arguments: "-E make_directory build"])
  dir('build') {
    cmake([
      installation: 'InSearchPath',
      arguments: '..'
    ])
  }
  cmakeBuild([
    installation: "InSearchPath",
    buildDir: 'build'
  ])
}

parallel tools: {
  node('tools') {
    stage('tools/scm') {
      deleteDir();
      checkoutGit();
    }
    stage('tools/cppcheck/create') {
      sh 'cppcheck --enable=all --xml --xml-version=2 -i ext common core tray 2> build/cppcheck.xml';
    }
    stage('tools/cloc/create') {
      sh 'cloc --by-file --exclude-dir=build,example,ext --xml --out=build/cloc.xml --quiet .';
    }
  } // node('tools')
},
linux_gcc: {
  node('linux' && 'gcc') {
    stage('linux-gcc/scm') {
      deleteDir();
      checkoutGit();
    }
    stage('linux-gcc/build') {
      build()
    }
  } // node('linux' && 'gcc')
},
linux_clang: {
  node('linux' && 'clang') {
    stage('linux-clang/scm') {
      deleteDir();
      checkoutGit();
    }
    stage('linux-clang/build') {
      build()
    }
  } // node('linux' && 'clang')
},
windows: {
    node('windows') {
    stage('windows/scm') {
      deleteDir();
      checkoutGit();
    }
    stage('windows/build') {
      build()
    }
  } // node('windows')
},
macos: {
  node('macos') {
    stage('macos/scm') {
      deleteDir();
      checkoutGit();
    }
    stage('macos/build') {
      build()
    }
  } // node('macos')
}
