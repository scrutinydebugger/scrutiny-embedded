pipeline {
    agent {
        label 'docker'
    }
    stages {
        stage ('Docker') {
            agent {
                dockerfile {
                    args '-e HOME=/tmp -e BUILD_CONTEXT=ci -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                    reuseNode true
                }
            }
            stages {
                stage ('Build and test (Native)') {
                    steps {
                        sh 'SCRUTINY_CI_PROFILE=build-test-native 
                        scripts/ci.sh'
                    }
                }
                stage ('Build AVR-GCC') {
                    steps {
                        sh 'SCRUTINY_CI_PROFILE=build-avr-gcc scripts/ci.sh'
                    }
                }
            }
        }
    }
}