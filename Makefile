# Makefile для nmake (VS Developer Command Prompt)
# Убедитесь, что файл называется Makefile и сохранён без BOM.

# путь к Вашему клону googletest
GTEST_BASE = C:\tools\googletest\googletest

# Include:
#   "."                             — для serializer.h
#   GTEST_BASE\include              — для gtest/gtest.h
#   GTEST_BASE                       — чтобы gtest-all.cc видел src/…
INCLUDES = /I"." \
           /I"$(GTEST_BASE)\include" \
           /I"$(GTEST_BASE)"

# исходники Google Test
GTEST_SRCS = \
    "$(GTEST_BASE)\src\gtest-all.cc" \
    "$(GTEST_BASE)\src\gtest_main.cc"

# компилятор и флаги
CXX      = cl
CXXFLAGS = /EHsc /std:c++20 $(INCLUDES)

# цель по умолчанию
all: serializer_tests.exe

# строим именно test_serializer.cpp (обратите внимание на имя!)
serializer_tests.exe: test_serializer.cpp serializer.h
	$(CXX) $(CXXFLAGS) test_serializer.cpp $(GTEST_SRCS) /Fe:serializer_tests.exe

clean:
	del serializer_tests.exe
	del *.obj
