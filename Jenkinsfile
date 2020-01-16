pipeline {
  agent any
  stages {
    stage('error') {
      parallel {
        stage('error') {
          steps {
            echo 'Start Building code'
            sh 'source setup.sh'
          }
        }

        stage('Building') {
          steps {
            sh 'mkdir build'
            sh 'cd build'
            sh 'cmake ..'
            sh 'make -2'
            sh 'source setup.sh'
          }
        }

      }
    }

  }
}