pipeline {
    agent {
        label 'docker'
    }
    stages {
        stage('All') {
            parallel{
                stage('GCC'){
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
                                CMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/gcc.cmake \
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
                stage('Clang'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-clang'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-clang -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    stages {
                        stage("Build") {
                            steps {
                                sh '''
                                CMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/clang.cmake \
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
                stage('GCC - No Datalogging'){
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
                                CMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/gcc.cmake \
                                SCRUTINY_BUILD_TEST=1 \
                                SCRUTINY_BUILD_TESTAPP=1 \
                                SCRUTINY_ENABLE_DATALOGGING=0 \
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
                            additionalBuildArgs '--target avr-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=avr-gcc -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    steps{
                        sh '''
                        CMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/avr-gcc.cmake \
                        SCRUTINY_BUILD_TEST=0 \
                        SCRUTINY_BUILD_TESTAPP=0 \
                        scripts/build.sh
                        '''
                    }
                }
                stage('aarch64-linux-gcc'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target aarch64-linux-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=aarch64-linux-gcc -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    steps{
                        sh '''
                        CMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/aarch64-linux-gcc.cmake \
                        SCRUTINY_BUILD_TEST=1 \
                        SCRUTINY_BUILD_TESTAPP=1 \
                        scripts/build.sh
                        '''
                    }
                }
                stage('powerpc64-linux-gcc'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target powerpc64-linux-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=powerpc64-linux-gcc -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    steps{
                        sh '''
                        CMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/powerpc64-linux-gcc.cmake \
                        SCRUTINY_BUILD_TEST=1 \
                        SCRUTINY_BUILD_TESTAPP=1 \
                        scripts/build.sh
                        '''
                    }
                }
                stage('arm-none-gcc'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target arm-none-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=arm-none-gcc -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                            reuseNode true
                        }
                    }
                    steps{
                        sh '''
                        CMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/arm-none-gcc.cmake \
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