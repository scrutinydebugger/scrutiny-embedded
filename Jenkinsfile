pipeline {
    agent {
        label 'docker'
    }
    stages {
        stage('Wait Docker'){
            agent {
                dockerfile {
                    additionalBuildArgs '--target CI'
                    reuseNode true
                }
            }

            steps {
                sh 'echo "ready"'
            }

        }
        stage('Compilers') {
            parallel{
                stage('Static Analysis'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target static-analysis'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=static-analysis'
                            reuseNode true
                        }
                    }
                    stages {
                        stage("Run") {
                            steps {
                                sh 'scripts/static_analysis.sh'
                            }
                        }

                    }
                }
                stage('GCC Full'){
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
                stage('GCC Full C++98'){
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
                                CMAKE_CXX_STANDARD=98   \
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
                stage('Clang Full'){
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
                        SCRUTINY_BUILD_CWRAPPER=0 \
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
        stage('Configs'){
            parallel{
                stage('GCC 64bits - No Datalogging'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-gcc-64bits-nodl -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
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
                                SCRUTINY_SUPPORT_64BITS=1 \
                                SCRUTINY_BUILD_CWRAPPER=1 \
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
                stage('GCC 64bits - No CWrapper'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-gcc-64bits-nocwrapper -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
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
                                SCRUTINY_ENABLE_DATALOGGING=1 \
                                SCRUTINY_SUPPORT_64BITS=1 \
                                SCRUTINY_BUILD_CWRAPPER=0 \
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
                stage('GCC 32bits - No Datalogging'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-gcc-32bits-nodl -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
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
                                SCRUTINY_SUPPORT_64BITS=0 \
                                SCRUTINY_BUILD_CWRAPPER=1 \
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
                stage('GCC 32bits'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-gcc-32bits-dl-buf16 -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
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
                                SCRUTINY_ENABLE_DATALOGGING=1 \
                                SCRUTINY_SUPPORT_64BITS=0 \
                                SCRUTINY_DATALOGGING_BUFFER_32BITS=0 \
                                SCRUTINY_BUILD_CWRAPPER=1 \
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
                stage('GCC 64bits - Datalogging Buffer 32bits'){
                    agent {
                        dockerfile {
                            additionalBuildArgs '--target native-gcc'
                            args '-e HOME=/tmp -e BUILD_CONTEXT=native-gcc-64bits-dl-buf32 -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
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
                                SCRUTINY_ENABLE_DATALOGGING=1 \
                                SCRUTINY_SUPPORT_64BITS=1 \
                                SCRUTINY_DATALOGGING_BUFFER_32BITS=1 \
                                SCRUTINY_BUILD_CWRAPPER=1 \
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
            }
        }
    }
}