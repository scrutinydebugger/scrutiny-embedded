pipeline {
    agent {
        label 'docker'
    }
    stages {
        stage('Parallel') {
            parallel{
                stage('Native GCC'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-gcc -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    stages {
                        stage("Build") {
                            steps {
                                sh '''
                                SCRUTINY_BUILD_TEST=1 \
                                SCRUTINY_BUILD_TESTAPP=1 \
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
                stage('Native CLang'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-clang'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-clang -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    steps{
                        sh '''
                        CMAKE_TOOLCHAIN_FILE=cmake/clang.cmake \
                        SCRUTINY_BUILD_TEST=1 \
                        SCRUTINY_BUILD_TESTAPP=1 \
                        scripts/build.sh
                        '''
                    }
                }
                stage('AVR'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target avr-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=avr-gcc -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    steps{
                        sh '''
                        CMAKE_TOOLCHAIN_FILE=cmake/avr-gcc.cmake \
                        SCRUTINY_BUILD_TEST=0 \
                        SCRUTINY_BUILD_TESTAPP=0 \
                        scripts/build.sh
                        '''
                    }
                }
                
            }
        }
    }
}