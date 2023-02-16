#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

set -u

WORKDIR=$(mktemp -d)
BINFILE=$1
TAGGED_FILE="$(dirname ${BINFILE})/$(basename ${BINFILE})_tagged"
SFDOUTPUT=$2
LOGLEVEL=debug

scrutiny get-firmware-id "${BINFILE}" --output "${WORKDIR}"  --loglevel ${LOGLEVEL}
scrutiny elf2varmap "${BINFILE}" --output "${WORKDIR}" --loglevel ${LOGLEVEL}
scrutiny make-metadata  --output "${WORKDIR}" --project-name "Scrutiny TestAPP" --author $(whoami) --version "1.0.0" --loglevel ${LOGLEVEL}
scrutiny add-alias "${WORKDIR}" --file "${SCRIPT_DIR}/../assets/aliases.json" --loglevel ${LOGLEVEL}

scrutiny tag-firmware-id "${BINFILE}" "${TAGGED_FILE}"  --loglevel ${LOGLEVEL}
chmod +x ${TAGGED_FILE}
echo "Tagged file written to ${TAGGED_FILE}"

scrutiny make-sfd "${WORKDIR}" "${SFDOUTPUT}" --loglevel ${LOGLEVEL}
scrutiny install-sfd "${SFDOUTPUT}" --loglevel ${LOGLEVEL}
