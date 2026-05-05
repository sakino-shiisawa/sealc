set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
	add_compile_options(/W4 /WX /permissive- /Zc:__cplusplus /Zc:preprocessor /EHsc /utf-8
		/w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311 /w14545
		/w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906
		/w14928
	)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wconversion -Wsign-conversion
		-Wshadow -Wnon-virtual-dtor -Wnull-dereference -fno-common -fstrict-aliasing
	)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
