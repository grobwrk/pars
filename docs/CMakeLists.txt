find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Find all sources to be documented
set(SRC_DIRS ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/example)
foreach(SRC_DIR ${SRC_DIRS})
  file(GLOB_RECURSE ITEMS ${SRC_DIR}/*.h ${SRC_DIR}/*.cpp)
  list(APPEND SRC_FILES ${ITEMS})
endforeach()

set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/lib)
set(DOXYGEN_INPUT_EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/example)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(
  OUTPUT ${DOXYGEN_INDEX_FILE}
  DEPENDS ${SRC_FILES}
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
  MAIN_DEPENDENCY ${DOXYFILE} ${DOXYFILE_IN}
  COMMENT "Generating docs"
  VERBATIM
)

# Nice named target so we can run the job easily
add_custom_target(doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
set_target_properties(doxygen PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/rst)
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

file(GLOB_RECURSE SPHINX_DEPS ${SPHINX_SOURCE}/*.rst)

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(
  OUTPUT ${SPHINX_INDEX_FILE}
  COMMAND
  ${SPHINX_EXECUTABLE} -b html
  # Tell Breathe where to find the Doxygen output
  -Dbreathe_projects.pars=${DOXYGEN_OUTPUT_DIR}/xml
  ${SPHINX_SOURCE} ${SPHINX_BUILD}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS
  # Other docs files you want to track should go here (or in some variable)
  ${SPHINX_DEPS}
  ${DOXYGEN_INDEX_FILE}
  MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
  COMMENT "Generating documentation with Sphinx"
)

# Nice named target so we can run the job easily
add_custom_target(docs DEPENDS ${SPHINX_INDEX_FILE})

install(DIRECTORY ${SPHINX_BUILD} ${DOXYGEN_OUTPUT_DIR} DESTINATION share/docs)
