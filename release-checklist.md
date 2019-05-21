## Release checklist
- Check whether the `doxygen` doesn't have unexpected warnings and errors while generating the documentation.
- Write the correct version into [win-bintray-export.bat](win-bintray-export.bat)
- Write the correct version into [win-stsff-export.bat](win-stsff-export.bat

#### [StsProjectDesc.cmake](cmake/StsProjectDesc.cmake)
- Set the correct version in the file and check other information there.

#### [conanfile.py](conanfile.py)
- Set the correct version.
- Check whether the file uses the correct dependencies and their version.
- Check whether the conan ```test_package``` works correctly.

