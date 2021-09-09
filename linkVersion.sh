#!/bin/bash


versions=$(ls -d v*)
_versions="latest $versions"
versions=""
prefix="["
for v in $_versions
do
	versions=$versions$prefix"'"$v"'"
	prefix=","
done
versions=$versions"]";

for v in $_versions
do
	sed -i "s/versions=.*/versions=$versions;/; s/version=.*/version='$v';/" $v/version_dropdown.js
done
