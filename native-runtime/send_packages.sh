#!/bin/bash

for i in {1..500}; do
  echo "Package ${i}";
  sudo sendip -p ipv4 -p udp -ud 1234 -d "z;THIS IS A PACKAGE SENT FROM THE COMMAND LINE" -v 127.0.0.1
  sleep 5.7
done
