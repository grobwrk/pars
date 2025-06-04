Tutorial
--------

Create an application from scratch
..................................

If you want to create an application that uses pars, you can conveniently use the ``pars-app-template``:

Using GitHub web
,,,,,,,,,,,,,,,,

1. open https://github.com/grobwrk/pars-app-template
2. click on ``Use this template``
3. then ``Create a new repository``
4. complete the procedure by naming your repository
5. follow the instructions in the ``Develop a pars application`` section

Using git
,,,,,,,,,

1. create the repository somewhere, say it's ``git@somewhere:somebody/my-pars-app.git``
2. clone the template ``git clone https://github.com/grobwrk/pars-app-template my-pars-app``
3. change directory to it ``cd my-pars-app``
4. set origin url ``git remote set-url origin git@somewhere:somebody/my-pars-app.git``
5. push set upstream ``git push -u origin``
6. follow the instructions in the ``Develop a pars application`` section

Integrate pars into your application
....................................

At the moment we only support vcpkg.

.. note::
   If you're not using vcpkg in your project, you're welcoming to contribute on extending the build systems that pars support.

Install vcpkg and my-vcpkg-triplets
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

1. install vcpkg (https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-bash)
2. make sure that you set ``VCPKG_ROOT`` environment variable
3. install my-vcpkg-triplets to ``${VCPKG_ROOT}/../my-vcpkg-triplets`` by cloning the repository (https://github.com/Neumann-A/my-vcpkg-triplets)

Add pars overlay to your project
,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

At the moment of writing the pars project is not yet published on the vcpkg repository. Follow these steps to add it as an overlay to your vcpkg base project:

1. create the folder ``ports/pars``
2. copy there the files ``portfile.cmake`` and ``vcpkg.json`` from the repository https://github.com/grobwrk/pars-app-template
3. enable the overlay by adding it in the ``overlay-ports`` of the ``vcpkg-configuration.json`` file:

.. code-block:: JSON

   "overlay-ports": [
     "./ports/pars"
   ]

Add pars port
,,,,,,,,,,,,,

1. run ``vcpkg add port pars``
2. include ``<pars/pars.h>``
3. build the project

Develop a pars application
..........................

Using CMake
,,,,,,,,,,,

1. change directory to where your project is
2. run ``cmake --list-presets``
3. choose a presets from the list, let's say ``linux-debug-llvm``
4. run ``cmake --preset linux-debug-llvm``
5. run ``cmake --build out/build/linux-debug-llvm``

Using Qt Creator
,,,,,,,,,,,,,,,,

This procesure was tested on ``Qt Creator 16.0.0``:

1. start ``Qt Creator``
2. navigate where the ``CMakeLists.txt`` of your project is
3. open it
4. on the configure page, select the LLVM kits
5. click ``Configure Project``
6. build the project

Using Visual Studio
,,,,,,,,,,,,,,,,,,,

This procedure was tested on ``Microsoft Visual Studio Community 2022 (64-bit)`` version ``17.13.6``.

1. start ``Visual Studio``
2. click ``Open a Local Folder``
3. open the root directory of your project
4. click ``Project > CMake Workspace Settings``
5. turn ``enableCmake`` from ``false`` to ``true``
6. save
7. build the project
