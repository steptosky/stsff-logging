::call conan create . stsff/develop ^
::     -s compiler="Visual Studio" ^
::     -s compiler.version=15 ^
::     -s compiler.runtime=MD ^
::     -s build_type=Release ^
::     -s arch=x86_64 ^
::     -o stsff-logging:shared=False ^
::     -e CONAN_BUILD_TESTING=1 ^
::     -e CTEST_OUTPUT_ON_FAILURE=1
::
::call conan create . stsff/develop ^
::     -s compiler="Visual Studio" ^
::     -s compiler.version=15 ^
::     -s compiler.runtime=MDd ^
::     -s build_type=Debug ^
::     -s arch=x86_64 ^
::     -o stsff-logging:shared=False ^
::     -e CONAN_BUILD_TESTING=1 ^
::     -e CTEST_OUTPUT_ON_FAILURE=1

call conan export . stsff/develop

pause
:: remove build dir in test package
:: rd /s/q "test_package/build"