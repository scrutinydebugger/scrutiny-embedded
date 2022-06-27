pipeline {
    agent {
        label 'docker'
    }
    stages {
        stage ('Docker') {
            agent {
                dockerfile {
                    additionalBuildArgs '--target builder'
                    args '-e HOME=/tmp -e BUILD_CONTEXT=ci -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache'
                    reuseNode true
                }
            }
            stages {
                stage ('Build') {
                    steps {
                        sh 'scripts/build.sh'
                    }
                }
                stage ('Test') {
                    steps {
                        sh 'scripts/runtests.sh'
                    }
                }
            }
        }
    }
}