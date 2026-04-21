#!/bin/bash
git checkout prd
git merge stg
git push origin prd
git checkout dev
