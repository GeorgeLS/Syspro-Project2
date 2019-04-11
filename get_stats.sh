#!/usr/bin/env bash

CLIENT_TAG="^\[C]"
CLIENT_LEFT_TAG="^\[L]"
SENDER_TAG="^\[S]"
RECEIVER_TAG="^\[R]"

# Dump stdin into a variable
INPUT=$(</dev/stdin)

# Get all rows with specified tags
CLIENT_ROWS=$(grep ${CLIENT_TAG} <<< "${INPUT}")
CLIENT_LEFT_ROWS=$(grep ${CLIENT_LEFT_TAG} <<< ${INPUT})
SENDER_ROWS=$(grep ${SENDER_TAG} <<< "${INPUT}")
RECEIVER_ROWS=$(grep ${RECEIVER_TAG} <<< "${INPUT}")

# Count the number of rows for each tag
CLIENTS_N=$(wc -l <<< ${CLIENT_ROWS})

if [[ -z "${CLIENT_LEFT_ROWS}" ]]; then
    CLIENTS_LEFT_N=0
else
    CLIENTS_LEFT_N=$(wc -l <<< ${CLIENT_LEFT_ROWS})
fi

FILES_SEND=$(wc -l <<< ${SENDER_ROWS})
FILES_RECEIVED=$(wc -l <<< ${RECEIVER_ROWS})

# Get the tokens of each sender row and compute the number of tokens
SENDER_TOKENS=( ${SENDER_ROWS} )
SENDER_TOKENS_N=$(( ${FILES_SEND} * 3 ))

BYTES_SEND=0
# Loop over sender tokens and get only the tokens with the bytes number and sum them
for (( I = 2; I < ${SENDER_TOKENS_N}; I=(( I + 3 )) )); do
    BYTES_SEND=`expr ${BYTES_SEND} + ${SENDER_TOKENS[I]}`
done

# Get the tokens of each receiver row and compute the number of tokens
RECEIVER_TOKENS=( ${RECEIVER_ROWS} )
RECEIVER_TOKENS_N=$(( ${FILES_RECEIVED} * 3 ))

BYTES_RECEIVED=0
# Loop over receiver tokens and get only the tokens with the bytes number and sum them
for (( I = 2; I < ${RECEIVER_TOKENS_N}; I=(( I + 3 )) )); do
    BYTES_RECEIVED=`expr ${BYTES_RECEIVED} + ${RECEIVER_TOKENS[I]}`
done

echo
echo "Number of Clients: ${CLIENTS_N}"
echo
printf "Client IDs: "

# Get the tokens of each client row and compute the number of tokens
CLIENT_TOKENS=( ${CLIENT_ROWS} )
CLIENT_TOKENS_N=$(( ${CLIENTS_N} * 2 ))

MAX_CLIENT_ID=${CLIENT_TOKENS[1]}
MIN_CLIENT_ID=${CLIENT_TOKENS[1]}

# Loop over the client tokens, print client IDs and compute max/min client ID
for (( I = 1; I < ${CLIENT_TOKENS_N}; I=(( I + 2 )) )); do
    if [[ (( ${CLIENT_TOKENS[I]} -gt ${MAX_CLIENT_ID} )) ]]; then
        MAX_CLIENT_ID=${CLIENT_TOKENS[I]}
    fi
    if [[ (( ${CLIENT_TOKENS[I]} -lt ${MIN_CLIENT_ID} )) ]]; then
        MIN_CLIENT_ID=${CLIENT_TOKENS[I]}
    fi
    printf "${CLIENT_TOKENS[I]} "
done

echo
echo
echo "Minimum Client ID: ${MIN_CLIENT_ID}"
echo
echo "Maximum Client ID: ${MAX_CLIENT_ID}"
echo
echo "Number of files send: ${FILES_SEND}"
echo
echo "Number of bytes send: ${BYTES_SEND}"
echo
echo "Number of files received: ${FILES_RECEIVED}"
echo
echo "Number of bytes received: ${BYTES_SEND}"
echo
echo "Number of Clients who left: ${CLIENTS_LEFT_N}"
echo