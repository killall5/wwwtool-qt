#!/bin/sh -e

cd /var/www/superiority
year=$(cat current)
mkdir -p $year
cd $year

while true; do
	/usr/local/bin/update_results.py
	sleep 1
done
