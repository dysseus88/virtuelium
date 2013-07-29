.. installation

Quickstart
==========

* **cmake installation**

  * for debian package:  ubuntu, debian, ... ::

    akc@pc-eisti-school: sudo apt-get install cmake

  * for rmp package:  fedora, ... ::

    akc@pc-eisti-school: yum install cmake

For this quickstart we assume the following prerequisites:

    * mesh_ecp was downloaded and extracted somewhere
    * cmake is installed
    * a compiler for c++ is installed (example: g++, c++, clang++, ...)


Installation and Building
=========================

Prerequisites
-------------

* You need to have :

  * CMake (minimum version 2.8)

    You can confirm that cmake 2.8 is installed by running *cmake --version* on the command line. For example on a Linux system,

      ::

        akc@pc-eisti-school: cmake --version
        cmake version 2.8.2



More options
------------

* For adding test:   cmake .. -DMesh_COMPILE_TEST=ON
* For adding examples:   cmake .. -DMesh_COMPILE_EXAMPLE=ON
* For adding tools:   cmake .. -DMesh_COMPILE_TOOL=ON
* For building documentation with doxygen:   cmake .. -DBUILD_DOC_DOXYGEN=ON
* For building sphinx documentation:   cmake .. -DBUILD_DOC_SPHINX=ON
* Build shared library:  cmake .. -DBUILD_SHARED_LIBS=ON  (by default it is set off)
* For html theme (sphinx documentation) : cmake .. -DSPHINX_HTML_THEME='themename'

  * Theme availables ::

     SPHINX_HTML_THEME=default, sphinxdoc, scrolls, agogo, traditional, nature, haiku


Examples
--------

Assuming that *bin* directory is created.

If you do the follow commands ::

  cd bin
  cmake .. -DMesh_COMPILE_EXAMPLE=ON -DMesh_COMPILE_TEST=ON
  -DMesh_COMPILE_TOOL=ON -DBUILD_DOC_DOXYGEN=ON


.. note:: If you want converting doxygen documentation to sphinx


Linux command line ::
  make doc

In ./sphinx ::

  make html

Standard commands, for sphinx Makefile

============ ============ ============ ============
============ ============ ============ ============
  changes     dirhtml      help         json
  linkcheck   pickle       text         clean
  doctest     html         latex        Makefile
  qthelp      devhelp      epub         htmlhelp
  latexpdf    man          singlehtml
============ ============ ============ ============

More Informations
-----------------

   * Template:  template<class T, class Idx>

     * 'T' for single and double precision (float / double)
     * 'Idx' for integer (int, int32_t, int64_t, ...)

How using MeshDriver
--------------------

