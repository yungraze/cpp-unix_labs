#!/bin/bash
git checkout stg
git merge dev
git push origin stg
git checkout dev
