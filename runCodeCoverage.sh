find . -type f -name "*.gcda" -print0 | xargs -I {} -0 rm -v "{}"

ctest --progress

lcov -c -d . -o main_coverage.info
lcov -r main_coverage.info "*/test/*" -o main_coverage.info
lcov -r main_coverage.info "*/.conan/*" -o main_coverage.info
lcov -r main_coverage.info "*MapEditor_autogen*" -o main_coverage.info
#lcov -r main_coverage.info "*mapEditor/*" -o main_coverage.info
lcov -r main_coverage.info "/usr*" -o main_coverage.info
lcov -r main_coverage.info "*fmt*" -o main_coverage.info
lcov -r main_coverage.info "*boost*" -o main_coverage.info
lcov -r main_coverage.info "*gtest*" -o main_coverage.info

genhtml main_coverage.info --output-directory out
#xdg-open out/index.html