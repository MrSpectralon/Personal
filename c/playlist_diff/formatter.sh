#!/bin/bash

# Use jq to format and store the output in payload.txt
jq . Data.json > payload.txt

# Notify user of success
echo "Formatted JSON has been saved to payload.txt"
