#!/bin/bash
git checkout stg
git merge dev -m "dev2stg"
git tag "stg_$(date +%Y%m%d_%H%M%S)"
git checkout dev
