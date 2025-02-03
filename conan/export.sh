#!/bin/bash

BASEDIR=$(dirname $0)

(cd ${BASEDIR}/spirv_reflect && conan export . --version 1.3.239.0)
(cd ${BASEDIR}/bin2header && conan export . --version 0.3.1)
