pipeline {
  agent any
  stages {
    stage('error') {
      parallel {
        stage('error') {
          steps {
            echo 'Start Building code'
          }
        }

        stage('Building') {
          steps {
            sh 'ls; ./setup.sh && echo CompileForHerd = $CompileForHerd - CompileForShep = $CompileForShep - UseRootFlag = $UseRootFlag; mkdir build && cd build && cmake .. &&  make -j2 VERBOSE=1;'
          }
        }

      }
    }

  }
}