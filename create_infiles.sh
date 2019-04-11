#!/usr/bin/env bash

GetRandomString() {
    RANDOM_STRING=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $(( ( $RANDOM % $2 ) + $1 )) | head -n 1)
}

# Main script code begins here

if [[ $# <  4 ]]; then
    echo "You must provide the following arguments: <dir_name> <num_of_files> <num_of_dirs> <levels>"
    exit 1
fi

DIR_NAME=$1
FILES_N=$2
DIR_N=$3
LEVELS=$4

if [[ ${FILES_N} < 1 ]]; then
    echo "You must provide a positive number of files"
    exit 1
fi

if [[ ${DIR_N} < 1 ]]; then
    echo "You must provide a positive number of directories"
    exit 1
fi

if [[ ${LEVELS} < 1 ]]; then
    echo "You must provide a positive number of levels"
    exit 1
fi

if [[ ! -d ${DIR_NAME} ]]; then
    mkdir ${DIR_NAME}
fi

DIR_NAMES_STR=""

for (( I = 0; I < ${DIR_N}; ++I )); do
    DIR_NAMES_STR+=" "
    GetRandomString 1 8
    DIR_NAMES_STR+=${RANDOM_STRING}
done

DIR_NAMES=( ${DIR_NAMES_STR} )

DIR_PATH="${DIR_NAME}/"
for (( I = 0; I < ${DIR_N}; ++I )); do
    if [[ $(( $I % ${LEVELS} )) -eq 0 && ! ( I -eq 0 ) ]]; then
        DIR_PATH="${DIR_NAME}/"
    fi
    DIR_PATH+=${DIR_NAMES[I]}
    mkdir ${DIR_PATH}
    DIR_PATH+="/"
done

FILE_NAMES_STR=""

for (( I = 0; I < ${FILES_N}; ++I )); do
    FILE_NAMES_STR+=" "
    GetRandomString 1 8
    FILE_NAMES_STR+=${RANDOM_STRING}
done

FILE_NAMES=( ${FILE_NAMES_STR} )

DIR_PATH="${DIR_NAME}"
J=0

MIN_BYTES=1024
MAX_BYTES=131072

for (( I = 0; I < ${FILES_N}; ++I )); do
    FILENAME="${DIR_PATH}/${FILE_NAMES[I]}"
    touch ${FILENAME}
    GetRandomString ${MIN_BYTES} ${MAX_BYTES}
    echo ${RANDOM_STRING} > ${FILENAME}
    if [[ $(( $J % ${LEVELS} )) -eq 0 && ! ( J -eq 0 ) ]]; then
        DIR_PATH="${DIR_NAME}/${DIR_NAMES[J]}"
    else
        DIR_PATH+="/${DIR_NAMES[J]}"
    fi

    (( ++J ))
    if [[ ${J} -gt ${DIR_N} ]]; then
        J=0
        DIR_PATH="${DIR_NAME}"
    fi
done

exit 0