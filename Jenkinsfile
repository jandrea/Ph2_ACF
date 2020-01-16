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
            sh 'mkdir build'
            sh 'cd build'
            sh 'cmake ..'
            sh 'make -2'
          }
        }

      }
    }

  }
}