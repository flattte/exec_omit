#!/usr/bin/env bash

./build_loader.sh $1
./build_target.sh

./bin/loader ./bin/target

