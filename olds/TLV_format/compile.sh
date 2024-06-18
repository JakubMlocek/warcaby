#!/bin/bash

gcc -I include game.c server_tlv.c -o server_tlv
gcc -I include game.c client_tlv.c -o client_tlv
