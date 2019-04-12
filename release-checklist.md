## Release checklist
- Check whether the `doxygen` does not have unexpected warnings and errors while generating the documentation.
- Check whether the dependencies and their version are described correctly in the [readme.md](readme.md) file. 

#### [StsProjectDesc.cmake](cmake/StsProjectDesc.cmake)
- Set the correct version in the file and check other information there.
- Check whether `ProjectVersionSnapshot` has the correct value for your branch.
  It should be `ON` for unstable branches like `feature, develop ...` and `OFF` for stable `master, release ...`.

#### [conanfile.py](conanfile.py)
- Set the correct version - for non stable branches it should be `snapshot` for stable branches a valid one.
- Check whether the file uses the correct dependencies and their version.
- Check whether the conan ```test_package``` works correctly.

---

## After Release

#### [StsProjectDesc.cmake](cmake/StsProjectDesc.cmake)
- Check whether `ProjectVersionSnapshot` has the correct value for your branch.
  It should be `ON` for unstable branches like `feature, develop ...` and `OFF` for stable like `master, release ...`.

#### [conanfile.py](conanfile.py)
- Set the correct version - for non stable branches it should be `snapshot` for stable the valid version.

---
