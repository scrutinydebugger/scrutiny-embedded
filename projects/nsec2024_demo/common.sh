#/bin/bash
RED='\033[0;31m'; CYAN='\033[0;36m'; YELLOW='\033[1;33m'; NC='\033[0m' 

info()  { >&2 echo -e "$CYAN[Info]$NC $1";}
warn()  { >&2 echo -e "$YELLOW[Warning]$NC $1";}
error() { >&2 echo -e "$RED[Error]$NC $1"; }
fatal() { >&2 echo -e "$RED[Fatal]$NC $1"; exit ${2:-1}; }
