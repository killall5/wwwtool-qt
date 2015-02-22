#!/bin/sh

mkdir -p /var/www/superiority/2015
cd /var/www/superiority/2015

while true; do
	/usr/local/bin/update_results.py
	sleep 1
done
