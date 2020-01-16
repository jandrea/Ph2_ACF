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
            sh 'mkdir build ; cd build ; cmake .. ; make -j2;'
          }
        }

      }
    }

  }
}