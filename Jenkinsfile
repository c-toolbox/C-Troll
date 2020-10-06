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

def build() {
  // createDirectory('build');
  // dir('build') {
  //   cmake([
  //     installation: 'InSearchPath',
  //     arguments: '..'
  //   ])
  // }

}

parallel tools: {
  node('tools') {
    stage('tools/scm') {
      deleteDir();
      checkoutGit();
    }
    // stage('tools/cppcheck/create') {
    //   createDirectory('build');
    //   sh 'cppcheck --enable=all --xml --xml-version=2 -i ext common core tray 2> build/cppcheck.xml';
    // }
    // stage('tools/cloc/create') {
    //   createDirectory('build');
    //   sh 'cloc --by-file --exclude-dir=build,example,ext --xml --out=build/cloc.xml --quiet .';
    // }
  } // node('tools')
},
linux_gcc: {
  node('linux' && 'gcc') {
    stage('linux-gcc/scm') {
      deleteDir();
      checkoutGit();
    }
    stage('linux-gcc/build') {
      cmakeBuild([
        buildDir: 'build',
        generator: 'Unix Makefiles',
        installation: "InSearchPath",
        steps: []
      ])
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
      cmakeBuild([
        buildDir: 'build',
        generator: 'Unix Makefiles',
        installation: "InSearchPath",
        steps: []
      ])

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
      cmakeBuild([
        buildDir: 'Visual Studio 16 2019 Win64',
        generator: 'Makefile',
        installation: "InSearchPath",
        steps: []
      ])

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
      cmakeBuild([
        buildDir: 'build',
        generator: 'Xcode',
        installation: "InSearchPath",
        steps: []
      ])

    }
  } // node('macos')
}
