message("python tests: ${PROJECT_BINARY_DIR}")

add_test (
  NAME python-tests
  COMMAND ${PYTHON_EXECUTABLE} -m pytest -v
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_tests_properties(
  python-tests
  PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_BINARY_DIR}/pybind:$ENV{PYTHONPATH}"
)
