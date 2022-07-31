environment {
    NATIVE_BUILD_FOLDER="build-native"
    AVR_GCC_BUILD_FOLDER="build-avr"
}

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
                stage('Testing'){
                    parallel{
                        stage('Native'){
                            steps("Build") {
                                sh '''
                                unset CMAKE_TOOLCHAIN_FILE
                                SCRUTINY_BUILD_TEST=1
                                SCRUTINY_BUILD_TESTAPP=1
                                SCRUTINY_BUILD_FOLDER=${NATIVE_BUILD_FOLDER}
                                scripts/build.sh
                                '''
                            }

                            steps("Test") {
                                sh '''
                                SCRUTINY_BUILD_FOLDER=${NATIVE_BUILD_FOLDER}
                                scripts/build.sh
                                '''
                            }
                        }

                        stage('AVR'){
                            steps{
                                sh '''
                                export CMAKE_TOOLCHAIN_FILE=cmake/avr-gcc.cmake
                                SCRUTINY_BUILD_TEST=0
                                SCRUTINY_BUILD_TESTAPP=0
                                SCRUTINY_BUILD_FOLDER=${AVR_GCC_BUILD_FOLDER}
                                scripts/build.sh
                                '''
                            }
                        }
                    }
                }
            }
        }
    }
}