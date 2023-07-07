#!/bin/bash

cp git_version.tmpl git_version.h
echo "#define GIT_VERSION \"$(git describe)\"" >> git_version.h