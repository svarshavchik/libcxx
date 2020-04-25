#! /bin/sh

./testoptgen				2>&1
./testoptgen -v				2>&1
./testoptgen -d				2>&1
./testoptgen -v -d			2>&1
./testoptgen --output foo		2>&1
./testoptgen --funcs			2>&1
./testoptgen -d --funcs			2>&1
./testoptgen --all			2>&1
./testoptgen --col1 --col2		2>&1
./testoptgen --all --not1		2>&1
./testoptgen --region x --region y	2>&1
./testoptgen --setflag			2>&1
./testoptgen --setflag=true foo bar	2>&1
./testoptgen --setflag=false		2>&1
./testoptgen --output			2>&1
