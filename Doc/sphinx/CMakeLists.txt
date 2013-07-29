
MESSAGE(STATUS "Available sphinx for ${PROJECT_NAME}")

SET(${PROJECT_NAME}_SRC_SPHINX_CONFIG_DIR ${${PROJECT_NAME}_SRC_SPHINX_DIR}/cmake)

# Generate documentation in HTML and PDF format using Sphinx (...).
SET(GENERATE_DOC TRUE)

FIND_PACKAGE(Sphinx REQUIRED)

# We use the Sphinx documentation generator to rENDer HTML and manual
# pages from the user and reference documentation in ReST format.
IF(NOT SPHINX_FOUND)
  message(WARNING "Unable to find Sphinx documentation generator")
  SET(GENERATE_DOC FALSE)
ENDIF(NOT SPHINX_FOUND)

# Sphinx uses latex and dvipng to rENDer LaTeX formulas.
find_package(LATEX QUIET)
IF(NOT LATEX_COMPILER)
  message(WARNING "Unable to find LaTeX compiler")
  SET(GENERATE_DOC FALSE)
ENDIF(NOT LATEX_COMPILER)
find_program(DVIPNG_CONVERTER dvipng)
IF(NOT DVIPNG_CONVERTER)
  message(WARNING "Unable to find dvipng converter")
  SET(GENERATE_DOC FALSE)
ENDIF(NOT DVIPNG_CONVERTER)
mark_as_advanced(DVIPNG_CONVERTER)

IF(NOT GENERATE_DOC)
  message(WARNING "Missing required documentation tools")
  return()
ENDIF()

SET(PREFIX_SPHINX sphinx_)

################################################################################

#option: default, sphinxdoc, scrolls, agogo, traditional, nature, haiku
IF(NOT DEFINED SPHINX_THEME)
  SET(SPHINX_THEME haiku)
ENDIF()

SET(SPHINX_THEME_DIR ${${PROJECT_NAME}_SRC_SPHINX_DIR}/_themes)
IF(NOT DEFINED SPHINX_THEME_DIR)
  SET(SPHINX_THEME_DIR)
ENDIF()

# sphinx source dir
SET(SPHINX_SRC_DIR "${${PROJECT_NAME}_SRC_SPHINX_DIR}/source")
# configured documentation tools and intermediate build results
SET(SPHINX_BUILD_DIR "${${PROJECT_NAME}_BIN_SPHINX_DIR}")
# Sphinx cache with pickled ReST documents
SET(SPHINX_CACHE_DIR "${${PROJECT_NAME}_BIN_SPHINX_DIR}/doctrees")

# sphinx external dir
SET(SPHINX_EXTERNAL_DIR "${${PROJECT_NAME}_SRC_SPHINX_DIR}/external")


################################################################################
SET(SPHINX_LOGO "${SPHINX_SRC_DIR}/_images/logo.png")
SET(SPHINX_ICO "${SPHINX_SRC_DIR}/_images/logo.ico")
SET(SPHINX_JSMATH_LOAD "./_static/jsMath/easy/load.js")

SET(SPHINX_PROJECT_NAME "${PROJECT_NAME}")
SET(SPHINX_COPYRIGHT_SUMMARY "${${PROJECT_NAME}_COPYRIGHT_SUMMARY}")
SET(SPHINX_NAMEFILE ${PROJECT_NAME}Libraryby${${PROJECT_NAME}_INSTITUTE_ADRV})

SET(SPHINX_HTMLHELP "${SPHINX_NAMEFILE}doc")

########### Latex
OPTION(${PROJECT_NAME}_SPHINX_LATEX_USE_PARTS
       "For manual sphinx documents, if this is true,
        then toplevel headings are parts,not chapters." ON)
OPTION(${PROJECT_NAME}_SPHINX_LATEX_SHOW_PAGEREFS
       "For manual sphinx documents,
        If true, show page references after internal links.." ON)
OPTION(${PROJECT_NAME}_SPHINX_LATEX_SHOW_URLS
       "For manual sphinx documents,
        If true, show URL addresses after external links." ON)
OPTION(${PROJECT_NAME}_SPHINX_LATEX_DOMAIN_INDICES
       "For manual sphinx documents,
        If false, no module index is generated." ON)

SET(SPHINX_LATEX_NAMEFILE "${SPHINX_NAMEFILE}.tex")
SET(SPHINX_LATEX_SUMMARY "${${PROJECT_NAME}_PROJECT_SUMMARY} Documentation")
SET(SPHINX_LATEX_INSTITUTE "${${PROJECT_NAME}_INSTITUTE_NAME}")
SET(SPHINX_LATEX_PAPER_SIZE a4)
SET(SPHINX_LATEX_FONT_SIZE 12pt)

SET(SPHINX_LATEX_OPTION
USE_PARTS
SHOW_PAGEREFS
SHOW_URLS
DOMAIN_INDICES
)
FOREACH(latex_option ${SPHINX_LATEX_OPTION})
SET(SPHINX_LATEX_${latex_option} False)
  IF(${PROJECT_NAME}_SPHINX_LATEX_${latex_option})
    SET(SPHINX_LATEX_${latex_option} True)
  ENDIF(${PROJECT_NAME}_SPHINX_LATEX_${latex_option})
ENDFOREACH(latex_option ${SPHINX_LATEX_OPTION})
SET(SPHINX_LATEX_USE_PARTS False)

########### HTML
OPTION(${PROJECT_NAME}_SPHINX_HTML_USE_SMARTYPANTS
       "For manual sphinx documents,
        If true, SmartyPants will be used to convert quotes and dashes to
        typographically correct entities." ON)
OPTION(${PROJECT_NAME}_SPHINX_HTML_DOMAIN_INDICES
       "For manual sphinx documents,
        If false, no module index is generated." ON)
OPTION(${PROJECT_NAME}_SPHINX_HTML_USE_INDEX
       "For manual sphinx documents,
        If false, no index is generated." OFF)
OPTION(${PROJECT_NAME}_SPHINX_HTML_SPLIT_INDEX
       "For manual sphinx documents,
        If true, the index is split into individual pages for each letter." OFF)
OPTION(${PROJECT_NAME}_SPHINX_HTML_SOURCE_LINK
       "For manual sphinx documents,
        If true, links to the reST sources are added to the pages." ON)
OPTION(${PROJECT_NAME}_SPHINX_HTML_SHOW_SPHINX
       "For manual sphinx documents,
        If true, Created using Sphinx is shown in the HTML footer.
        Default is True." OFF)
OPTION(${PROJECT_NAME}_SPHINX_HTML_SHOW_COPYRIGHT
       "For manual sphinx documents,
        If true, (C) Copyright ... is shown in the HTML footer.
        Default is True." ON)

SET(SPHINX_HTML_OPTION
USE_SMARTYPANTS
DOMAIN_INDICES
USE_INDEX
SPLIT_INDEX
SOURCE_LINK
SHOW_SPHINX
SHOW_COPYRIGHT
)

FOREACH(html_option ${SPHINX_HTML_OPTION})
SET(SPHINX_HTML_${html_option} False)
  IF(${PROJECT_NAME}_SPHINX_HTML_${html_option})
    SET(SPHINX_HTML_${html_option} True)
  ENDIF(${PROJECT_NAME}_SPHINX_HTML_${html_option})
ENDFOREACH(html_option ${SPHINX_HTML_OPTION})


########### Man Page
SET(SPHINX_MANPAGE_NAMEFILE "${SPHINX_NAMEFILE}")
SET(SPHINX_MANPAGE_SUMMARY "${${PROJECT_NAME}_PROJECT_SUMMARY} Documentation")

########### Xml doxygen
SET(SPHINX_XML_DOXYGEN "${${PROJECT_NAME}_BIN_DOXY_DIR}/xml")

########### EPUB
OPTION(${PROJECT_NAME}_SPHINX_EPUB_TOCDUP
       "For manual sphinx documents,
        Allow duplicate toc entries." ON)

SET(SPHINX_EPUB_TITLE ${SPHINX_PROJECT_NAME})
SET(SPHINX_EPUB_AUTHOR ${${PROJECT_NAME}_INSTITUTE_NAME})
SET(SPHINX_EPUB_PUBLISHER ${${PROJECT_NAME}_INSTITUTE_NAME})
SET(SPHINX_EPUB_COPYRIGHT ${SPHINX_COPYRIGHT_SUMMARY})
SET(SPHINX_EPUB_TOCDUP False)
IF(${PROJECT_NAME}_SPHINX_EPUB_TOCDUP)
  SET(SPHINX_EPUB_TOCDUP True)
ENDIF(${PROJECT_NAME}_SPHINX_EPUB_TOCDUP)



IF(EXISTS ${SPHINX_XML_DOXYGEN})
  SET(PATH_XML_DOXYGEN "${${PROJECT_NAME}_BIN_DOXY_DIR}/xml")
  SET(name_doxygen_rst_file doxygen.breathe.rst)
ELSE()
  SET(name_doxygen_rst_file doxygen.rst)
ENDIF(EXISTS ${SPHINX_XML_DOXYGEN})


CONFIGURE_FILE(${${PROJECT_NAME}_SRC_SPHINX_CONFIG_DIR}/conf.py.cmake
               ${${PROJECT_NAME}_SRC_SPHINX_DIR}/source/conf.py)
CONFIGURE_FILE(${${PROJECT_NAME}_SRC_SPHINX_CONFIG_DIR}/${name_doxygen_rst_file}
               ${${PROJECT_NAME}_SRC_SPHINX_DIR}/source/doxygen.rst)



################################################################################

# You can set these variables from the COMMAND line.
SET(SPHINXOPTS)
SET(PAPER a4)

# Internal variables.
SET(PAPEROPT_a4 -D latex_paper_size=a4)
SET(PAPEROPT_letter -D latex_paper_size=letter)
SET(ALLSPHINXOPTS  -d ${SPHINX_BUILD_DIR}/doctrees ${PAPEROPT_${PAPER}} ${SPHINXOPTS} ${SPHINX_SRC_DIR})

#  @echo "Please use \`make <TARGET>' where <TARGET> is one of"
#  @echo "  html       to make standalone HTML files"
#  @echo "  dirhtml    to make HTML files named index.html in directories"
#  @echo "  singlehtml to make a single large HTML file"
#  @echo "  pickle     to make pickle files"
#  @echo "  json       to make JSON files"
#  @echo "  htmlhelp   to make HTML files and a HTML help project"
#  @echo "  qthelp     to make HTML files and a qthelp project"
#  @echo "  devhelp    to make HTML files and a Devhelp project"
#  @echo "  epub       to make an epub"
#  @echo "  latex      to make LaTeX files, you can set PAPER=a4 or PAPER=letter"
#  @echo "  latexpdf   to make LaTeX files and run them through pdflatex"
#  @echo "  text       to make text files"
#  @echo "  man        to make manual pages"
#  @echo "  changes    to make an overview of all changed/added/deprecated items"
#  @echo "  linkcheck  to check all external links for integrity"
#  @echo "  doctest    to run all doctests embedded in the documentation (if enabled)"


SET(SPHINX_MAKEFILE_OPTION
 html dirhtml singlehtml pickled json htmlhelp qthelp
 devhelp epub latex text man changes linkcheck doctest) #latexpdf

SET(linkcheck_comment "Link check complete; look for any errors in the above output or in ${SPHINX_BUILD_DIR}/linkcheck/output.txt.")
SET(changes_comment "The overview file is in ${SPHINX_BUILD_DIR}/changes.")
SET(doctest_comment "Testing of doctests in the sources finished, look at the results in ${BUILDDIRR}/doctest/output.txt.")
SET(htmlhelp_comment "Build finished; now you can run HTML Help Workshop with the .hhp project file in ${SPHINX_BUILD_DIR}/htmlhelp.")
SET(qthelp_comment "
Build finished; now you can run \"qcollectiongenerator\" with the .qhcp project file in ${SPHINX_BUILD_DIR}/qthelp, like this:
# qcollectiongenerator ${SPHINX_BUILD_DIR}/qthelp/NeXusacommondataformatforneutronx-rayandmuonscience.qhcp
To view the help file:
# assistant -collectionFile ${SPHINX_BUILD_DIR}/qthelp/NeXusacommondataformatforneutronx-rayandmuonscience.qhc
")
SET(devhelp_comment "
Build finished.
To view the help file:
# mkdir -p $$HOME/.local/share/devhelp/NeXusacommondataformatforneutronx-rayandmuonscience
# ln -s ${SPHINX_BUILD_DIR}/devhelp $$HOME/.local/share/devhelp/NeXusacommondataformatforneutronx-rayandmuonscience
# devhelp
")
SET(latex_comment "Build finished; the LaTeX files are in ${SPHINX_BUILD_DIR}/latex. Run 'make' in that directory to run these through (pdf)latex (use 'make latexpdf' here to do that automatically).")

FOREACH(VAL ${SPHINX_MAKEFILE_OPTION})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${SPHINX_BUILD_DIR}/${VAL}
        COMMAND ${SPHINX_EXECUTABLE}
        ARGS -b ${VAL} ${ALLSPHINXOPTS} ${SPHINX_BUILD_DIR}/${VAL}
        DEPENDS ${SPHINX_SRC_DIR}
        COMMENT   "${${VAL}_comment}"
    )
    ADD_CUSTOM_TARGET(${PREFIX_SPHINX}${VAL} DEPENDS "${SPHINX_BUILD_DIR}/${VAL}")
ENDFOREACH(VAL)


ADD_CUSTOM_TARGET(${PREFIX_SPHINX}complete COMMAND cp -f ${SPHINX_BUILD_DIR}/latex/*.pdf ${SPHINX_BUILD_DIR}/html DEPENDS html latexpdf COMMENT "Build finished. The HTML and PDF pages are in ${SPHINX_BUILD_DIR}/html.")

ADD_CUSTOM_TARGET(${PREFIX_SPHINX}latexpdf COMMAND make -C ${SPHINX_BUILD_DIR}/latex all-pdf DEPENDS latex COMMENT "Running latex through pdflatex")

