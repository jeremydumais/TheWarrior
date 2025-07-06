find . -type f -name "*.gcda" -print0 | xargs -I {} -0 rm -v "{}"

ctest --progress --parallel 8

lcov -c -d . -o main_coverage.info --ignore-errors mismatch
lcov -r main_coverage.info "*/test/*" -o main_coverage.info
lcov -r main_coverage.info "*/.conan/*" -o main_coverage.info
lcov -r main_coverage.info "*MapEditor_autogen*" -o main_coverage.info
lcov -r main_coverage.info "*ItemEditor_autogen*" -o main_coverage.info
lcov -r main_coverage.info "/usr*" -o main_coverage.info
lcov -r main_coverage.info "*fmt*" -o main_coverage.info
lcov -r main_coverage.info "*boost*" -o main_coverage.info
lcov -r main_coverage.info "*gtest*" -o main_coverage.info

genhtml main_coverage.info --output-directory out
