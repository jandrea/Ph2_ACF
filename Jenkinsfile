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
            sh 'pwd'
            sh 'ls'
            sh 'make -j2'
          }
        }

      }
    }

  }
}