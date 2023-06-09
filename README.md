AutoRDF is an original open source framework used to make it a lot easier to work with RDF data from a software engineering point of view.
Building on top of the Redland software package, it bridges the gap between semantic web ontology and legacy object oriented languages,
by providing transparent access to RDF resources from within standard C++ objects. Its use of widespread C++11, Boost and Redland  make it
suitable not only for the desktop and server, but also to low computing power embedded devices.

More info at https://github.com/ariadnext/AutoRDF/wiki

[![Build Status](https://travis-ci.org/ariadnext/AutoRDF.svg?branch=master)](https://travis-ci.org/ariadnext/AutoRDF)


In order to generate the Conda Packaging, here are the steps:
- Run the docker container from the the Dockerfile ./docker/Dockerfile.buildconda mount the current repository as a volume
```
docker build --build-arg irma_rsa= <Gitlab Private Key> \
             --build-arg irma_rsa_pub= <Gitlab Public Key> \
             --build-arg known_hosts= <Gitlab Known Host>  \
             -t autordf_conda_packaging -f ./docker/Dockerfile.buildconda .
docker run -it -v ./:/workspaces autordf_conda_packaging:latest
cd /workspaces
conda build recipe
```
- The Conda Python Package is now available under: /home/developer/conda-bld/linux-64/autordf-1.0.0-py310h3fd9d12_0.tar.bz2
Note that the conda build can be exectued by using Dockerfile.buildconda as a Dev Container.
