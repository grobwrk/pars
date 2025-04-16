.. _building:

Building
========

Install Deps
------------

Follow these instructions (for Ubuntu 24.10) to install the dependencies to build the framework and example applications:

- ``sudo apt install build-essential cmake ninja-build pkg-config git``
- install vcpkg to ``~/vcpkg`` (https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-bash)
- install my-vcpkg-triplets to ``~/my-vcpkg-triplets`` by cloning the repository (https://github.com/Neumann-A/my-vcpkg-triplets)

Clone
-----

- ``git clone https://github.com/grobx/pars.git ~/pars``

Build
-----

- ``cd ~/pars``
- ``cmake --preset linux-release-llvm``
- ``cmake --build out/build/linux-release-llvm``

Run
---

- ``cd ~/pars/out/build/linux-release-llvm/examples``

Run a server backend that listen on port 49001 and react to events that receives from clients:

- ``./server_backend listen tcp://0.0.0.0:49001``

Execute a client that ask the server backend to compute the fibonacci number of 42 using a fast algorithm.

- ``./client dial tcp://127.0.0.1:49001 1 1 42``

Execute a client that ask the server backend to compute the fibonacci number of 42 using a slow algorithm (you can cancel the computation by hitting ``CTRL-C``).

- ``./client dial tcp://127.0.0.1:49001 1 0 42``

For further usages run ``./client`` or ``./server_backend``.
