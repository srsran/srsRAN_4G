#!/bin/bash

txt2man -d "${CHANGELOG_DATE}" -t SRSENB -s 8 srsenb.txt > srsenb.8
txt2man -d "${CHANGELOG_DATE}" -t SRSEPC -s 8 srsepc.txt > srsepc.8
txt2man -d "${CHANGELOG_DATE}" -t SRSUE  -s 8 srsue.txt > srsue.8
