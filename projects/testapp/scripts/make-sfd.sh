#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

set -u

WORKDIR=$(mktemp -d)
BINFILE=$1
SFDOUTPUT=$2
LOGLEVEL=debug

scrutiny get-firmware-id "${BINFILE}" --output "${WORKDIR}" --apply  --loglevel ${LOGLEVEL}
scrutiny elf2varmap "${BINFILE}" --output "${WORKDIR}" --loglevel ${LOGLEVEL}
scrutiny make-metadata  --output "${WORKDIR}" --project-name "Scrutiny TestAPP" --author $(whoami) --version "1.0.0" --loglevel ${LOGLEVEL}

scrutiny add-alias "${WORKDIR}" --target "/rpv/x5000" --fullpath "/alias/rpv5000(enable)" --loglevel ${LOGLEVEL}
scrutiny add-alias "${WORKDIR}" --target "/rpv/x5001" --fullpath "/alias/rpv5001(counter)" --loglevel ${LOGLEVEL}

scrutiny add-alias "${WORKDIR}" --target "/static/main.cpp/process_interactive_data()/enable" --fullpath "/alias/enable_var" --loglevel ${LOGLEVEL}
scrutiny add-alias "${WORKDIR}" --target "/static/main.cpp/process_interactive_data()/counter" --fullpath "/alias/counter_var" --loglevel ${LOGLEVEL}
scrutiny add-alias "${WORKDIR}" --target "/static/main.cpp/process_interactive_data()/counter" --fullpath "/alias/counter_var_x2+1000" --gain 2 --offset 1000 --loglevel ${LOGLEVEL}

scrutiny make-sfd "${WORKDIR}" "${SFDOUTPUT}" --loglevel ${LOGLEVEL}
scrutiny install-sfd "${SFDOUTPUT}" --loglevel ${LOGLEVEL}
