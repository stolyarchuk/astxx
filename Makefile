prefix = /usr
exec_prefix = ${prefix}/local
libdir = ${exec_prefix}/lib

make: all

clean:
	rm -rf build
	rm -rf output   	

all: 
	mkdir -p build
	mkdir -p output	

	$(CXX) "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/1-basic_action.o" "src/manager/basic_action.cc"
	$(CXX) "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/2-connection.o" "src/manager/connection.cc"
	$(CXX) "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/3-error.o" "src/manager/error.cc"
	$(CXX) "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/4-error_code.o" "src/boost/system/error_code.cpp"

	ar cr "output/libastxx_manager.a" "build/1-basic_action.o" "build/2-connection.o" "build/3-error.o" "build/4-error_code.o" 	
	ranlib "output/libastxx_manager.a"

	$(CXX) "-fPIC" "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/5-basic_action.o" "src/manager/basic_action.cc"
	$(CXX) "-fPIC" "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/6-connection.o" "src/manager/connection.cc"
	$(CXX) "-fPIC" "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/7-error.o" "src/manager/error.cc"
	$(CXX) "-fPIC" "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/8-error_code.o" "src/boost/system/error_code.cpp"
	$(CXX) "-shared" "-Wl,-soname,libastxx_manager.so.0"   -o "output/libastxx_manager.so.0.6" "build/5-basic_action.o" "build/6-connection.o" "build/7-error.o" "build/8-error_code.o" "-lboost_signals" "-lpthread"

	$(CXX) "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/10-agi.o" "src/agi.cc"

	ar cr "output/libastxx_agi.a" "build/10-agi.o"
	ranlib "output/libastxx_agi.a"

	$(CXX) "-fPIC" "-Wall" "-ggdb3"  "-Iinclude"   -c -o "build/11-agi.o" "src/agi.cc"

	$(CXX) "-Wall" "-ggdb3" "-shared" "-Wl,-soname,libastxx_agi.so.0"   -o "output/libastxx_agi.so.0.6" "build/11-agi.o" 

install:
	@if [ ! -d output ]; then \
           echo "you should run 'make' or 'make all' first"; exit 1;  \
        fi
	
	cp output/* ${libdir}
