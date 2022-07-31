pipeline {
    agent {
        label 'docker'
    }
    stages {
        stage('Parallel') {
            parallel{
                stage('Native'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    stages {
                        stage("Build") {
                            steps {
                                sh '''
                                export SCRUTINY_BUILD_TEST=1
                                export SCRUTINY_BUILD_TESTAPP=1
                                scripts/build.sh
                                '''
                            }
                        }
                        stage("Test") {
                            steps {
                                sh '''
                                scripts/runtests.sh
                                '''
                            }
                        }
                    }
                }
                stage('AVR'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target avr'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=avr -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    steps{
                        sh '''
                        export CMAKE_TOOLCHAIN_FILE=cmake/avr-gcc.cmake
                        export SCRUTINY_BUILD_TEST=0
                        export SCRUTINY_BUILD_TESTAPP=0
                        scripts/build.sh
                        '''
                    }
                }
            }
        }
    }
}