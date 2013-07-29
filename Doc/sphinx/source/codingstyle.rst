.. highlightlang:: c++

.. http://docutils.sourceforge.net/docs/user/rst/quickref.html
.. http://sphinx.pocoo.org/contents.html

Coding Covention
=================

Naming convention
-----------------

There is three types of case usable:

Lower case
  all the letter are lower case, the word are separated by _ as ``ma_variable``

Mixed case
  The first letter of each word are uper case, the other lower (even on
  accronyme) as in ``HtmlWriter``

Upper case
  All the letter are upper case, the word are separated by _ like ``BLIP_ERR``

.. .............................................................................

Constants are: global const variable, static const member variable, enum value.

.. Summary of the naming convention used

.. .. table:: Summary of the naming convention used

====================== ==================== ==============================
identifier type        convention           example
====================== ==================== ==============================
Public member variable lower case           ``bad_variable``
Member variable        lower case, prefixed ``m_vector_size``
                       with ``m_``
Static member variable lower case, prefixed ``s_instance_number``
                       with ``s_``
Local variable         lower case           ``count``
Global variable        lower case, prefixed ``g_bad_variable``
                       with ``g_``
Constante              Upper case, prefixed ``c_MAX_SIZE``
                       with ``c_``
Function               Mixed case, starting ``int UpdateAccount()``
                       with a verb
Class                  Mixed case           ``class DenseMatrix;``
typedef                Mixed case           ``typedef [...] Complex;``
Namespace              Mixed case           ``namespace LinearAlgebra {}``
Macro                  Upper case           ``#define BLIP_LOG(msg)``
====================== ==================== ==============================

exeption for public variable member (only on POD ;) ) no prefix


Identifiers should be meaningful. That is, they should be easy to understand
and provide good documentation about themselves. Avoid abbreviations,
especially ad hoc ones.

Single character variable names should be avoided because of the difficulty of
maintaining code that uses them. However, single character names may be
appropriate for variables that are essentially meaningless, such as dummy loop
counters with short loop bodies or temporary pointer variables with short
lifetimes.

Programming rules
-----------------

.. warning::

  Do not use ``goto``. In loop, you may want to use ``break`` and ``continue``.

.. note:: Try to not define new macro.

.. warning:: Do not import namespace into global or library scope.

.. warning:: Do not use global variable.

.. note::

  Put guard one our header file, use the name of the file all upercase
  between ``GUARD_`` and ``_H_``. If you are working on a library, you may want
  to put his name before the file name. See exemple below::

    // this is the file 'MonFichier.h'
    #ifndef GUARD_MONFICHIER_H_
    #define GUARD_MONFICHIER_H_

    // your code goes here

    #endif // #ifndef GUARD_MONFICHIER_H_


Try to make your build clean: there should be no warning.

Global variables must be preceded with a "g _ " (avoid global declarations) ::

  int g_nbiter;
  double g_residu;

  void DisplayIndex() {
    int i;
    i=g_nbiter;
    printf("index = %i",i);
  }

Do not define enumeration at global or library scope: put then either in
the class which use them or in a namespace. See below::

  // do not do this:
  // enum Color { red,blue,green };

  // do this if there is a logical link between the class and the enum
  class MaClass {
  public:
    enum Color { c_RED, c_BLUE, c_GREEN };
  }

  // or this
  namespace Color {
    enum ColorEnum { c_RED, c_BLUE, c_GREEN };
  }

  // acces to the enumarated value is then:
  // MaClass::c_BLUE or Color::c_GREEN

.. note::

  Do not use magic number: Do not use litteral number directly, define a
  named constante and use it::

    void Fct() {
      // do not do
      //double buf[100]
      // instead do
      const int c_BUF_SIZE = 100;
      double buf[c_BUF_SIZE];
      // then later on:
      for(int i=0; i<c_BUF_SIZE; ++i) {}
      // we can immediatly see to what refer the c_BUF_SIZE
      // it wouldn't be the cas if you do
      //for(int i=0; i<100; ++i) {}
      // the worst case is if you have multiple constante with the same
      // value but different meaning. Try then to change one and not the others
    }


For **pointer varibales**, place the *** close to the pointer type not
the variable name ::

   int* array_of_residu; //okay

   int *array_of_residu; //not allowed

We apply the same rules for reference variables ::

   int& array_of_residu; //okay

   int &array_of_residu; //not allowed

Indenting, brace, line length
-----------------------------

The file should be indented with 2 space.

The opening brace is placed at the end of the line::

  class Test {
    void Fct() {
      if(b) {
      }
    }
  }

.. note:: A line should note be longer than 80 characteres.

In some case, the brace can be placed on a new line, If it make things more
readable::

  void UneFonctionAvecUnNomTresLong(int and_lots, int of, int params, int so,
                                    int that, int it, int take,
                                    int more_than_80_chars)
  {
  }

  MaClass::MaClass(int long_name_foobar, int another_one_barfoo)
    :m_long_name_foobar(long_name_foobar)
    ,m_another_one_barfoo(another_one_barfoo)
  {
  }


Commenting the code
-------------------

In the header files, document your code using doxygen style comment. you can
find it doxygen and its manual here: http://www.stack.nl/~dimitri/doxygen/.

Here is a small example::

  //! Represent a dense vector
  class DenseVector {
  public:
    //! Construct the vector by giving its bound
    /*! The vector will be filled with 0
     *
     *  \param start the lower bound of the vector
     *  \param stop  the upper bound of the vector
     */
    DenseVector(int start, int stop);

    //! Brief Description
    /*! A longer description
     *  - caret can be used to make list
     *  - item 2
     *  - item 3
     *
     *  \param i param descr
     *  \return description of returned value
     *  \pre precondition
     *  \post postcondition
     *  \warning something to be cautious about
     *  \throw exception_object descr
     *
     *  \todo Something todo
     */
    double DoSomething(int i);

    //! used on typedef
    typedef double Scalar;
  private:
    //! used on variable
    int m_start;
  };

The following syntax can also be used::

  /** Short description only, do not put a point at the end */
  class DenseVector {
  public:
    /** The short description here.
     *  The detailed description here
     */
    DenseVector(int start, int stop);

    /** Short descr can span multiple line,
     *  they stop at the first point
     */
    typedef double Scalar;
  }

for more information refer to the manual of doxygen.

