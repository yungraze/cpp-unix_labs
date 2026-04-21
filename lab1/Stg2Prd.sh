
#!/bin/bash

git checkout prd

git merge stg -m "stg2prd"

git tag "prd_$(date +%Y%m%d_%H%M%S)"

git checkout dev

