.. using

How using this library
======================

File format
-----------

* **.vtk** file

  * structure ::

  * example ::


Simple code using our library
-----------------------------

Example 1 (using simple CPU functions)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

  #include "Mesh.hpp"

  int main (int argc, char *argv[])
  {

  return 0;
  }


For using our libraries, include "**Mesh.hpp**" header file.

 * How compile this minimal program:

   * Assuming the following tree folder ::

    ./examples
    |-- example1
    |   |-- example1.cpp
    |-- ...

   * example 1 implementation is in *example1.cpp* file.
   * headers files are in **/home/akca/hpcteamecp/include**
   * libraries files are in **/home/akca/hpcteamecp/lib**

   ::

     g++ -o myexec.x example1.cpp -I/home/akca/hpcteamecp/include

Example 2 (loading data functions)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

  #include "Mesh.hpp"

  int main (int argc, char *argv[])
  {

  return 0;
  }


The **example 2** show how create a mesh from a vtk file.

 * How compile this minimal program:

   * Assuming the following tree folder ::

    ./examples
    |-- example1
    |   |-- example1.cpp
    |-- example2
    |   |-- example2.cpp
    |-- ...

   * example 2 implementation is in *example2.cpp* file.
   * headers files are in **/home/akca/hpcteamecp/include**
   * libraries files are in **/home/akca/hpcteamecp/lib**

  ::

     g++ -o myexec.x example1.cpp -I/home/akca/hpcteamecp/include
     -L /home/akca/hpcteamecp/lib -lMesh -lBlip


 **Remark:** example 2 use Blip and Mesh functions.


Cmake recommend
----------------

We assume prerequisites are installed.

We recommend to use **CMake** for compilation (very simple ^0^)

  * Step 1 :  create directories

    * **root directory**:  *simple_mesh/* ::

      akca@pc-ecp-mas-hpc: mkdir simple_mesh

    * **source directory** : src in *simple_mesh/* ::

      akca@pc-ecp-mas-hpc: cd simple_mesh

      akca@pc-ecp-mas-hpc: mkdir src

    * **binary directory** : bin in *simple_mesh/* ::

      akca@pc-ecp-mas-hpc: cd simple_mesh

      akca@pc-ecp-mas-hpc: mkdir bin

  * Step 2 : go to **src/** directory ::

      akca@pc-ecp-mas-hpc: cd src


  * Step 3 : create  CMakeLists.txt file and put the follow code ::

  ## Project
  CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

  set(project_name simple_mesh)

  project(${project_name})

  #examples path (change by our own paths)

  #include  (include directory)

  include_directories(/home/akca/hpcteamecp/include)

  #add executable

  ADD_EXECUTABLE(example3.x example3.cpp)

  #link libraries of our executable (example3.x)

  TARGET_LINK_LIBRARIES(example3.x Blip Mesh)

  * Step 4 : go to **bin** directory ::

      akca@pc-ecp-mas-hpc: cd bin


  * Step 5 : Run **cmake**  ::

      akca@pc-ecp-mas-hpc: cmake ../src

  * Step 6 : Run **make**  ::

      akca@pc-ecp-mas-hpc: make

This will run compilation over the example code. View the results (executable) at: ::

  ./simple_mesh/bin/src/

