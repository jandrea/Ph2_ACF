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
            sh 'cd build'
            sh 'cmake ..'
            sh 'make -j2'
          }
        }

      }
    }

  }
}