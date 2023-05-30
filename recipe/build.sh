#!/bin/bash

# Directory where subprojects will be clone and build
subprojects_dir="subprojects"

#PREFIX and SP_DIR are set directly by the conda builder
echo "Prefix: ${PREFIX}"
echo "Python Site Package: ${SP_DIR}"

git_checkout_branch_serd="debian-axt"
git_checkout_branch_sord="debian-axt"

# Function to create directory if it doesn't exist
function create_dir_if_not_exists {
    if [ ! -d "$1" ]; then
        mkdir -p "$1"
    fi
}

# Function to clone repo if directory doesn't exist
function clone_repo_if_not_exists {
    if [ ! -d "$1" ]; then
        create_dir_if_not_exists "$1"
        echo git@gitlab.rennes.ariadnext.com:$2
        git -c http.sslVerify=false clone  git@gitlab.rennes.ariadnext.com:$2 "$1"
    fi
}

# Function for building and installing a CMake Project
function build_and_install {
    cd "$1"
    git checkout "$2"
    if [ ! -d builddir ]; then
        mkdir builddir
    else 
        rm -r builddir
        mkdir builddir
    fi
    cd builddir
    cmake .. -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_PREFIX_PATH=${PREFIX}
    make
    make install
    cd ../../..
}

create_dir_if_not_exists "$subprojects_dir"
clone_repo_if_not_exists "$subprojects_dir/serd" "ocr/serd-mirror.git"
clone_repo_if_not_exists "$subprojects_dir/sord" "ocr/sord-mirror.git"


build_and_install "$subprojects_dir/serd" "$git_checkout_branch_serd"
export PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig/:$PKG_CONFIG_PATH"
build_and_install "$subprojects_dir/sord" "$git_checkout_branch_sord"


create_dir_if_not_exists "builddir"
cd builddir
cmake .. -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_PREFIX_PATH=${PREFIX} -DRDF_C_API=SORD -DBUILD_DOC=1 -DGENERATE_PYTHON_BINDINGS=1 -DCONDA_PACKAGE_GENERATION=true -DPYTHON_SITE_PACKAGE=${SP_DIR}
make
make install
