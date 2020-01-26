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
            sh 'ls setup.sh$$ ./setup.sh$$ ls $$ mkdir build $$ cd build $$ cmake .. $$  make -j2 VERBOSE=1;'
          }
        }

      }
    }

  }
}