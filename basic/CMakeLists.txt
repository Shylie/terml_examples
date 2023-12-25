cmake_minimum_required(VERSION 3.18)

add_executable(terml_test_basic
	main.cpp
)

set_target_properties(terml_test_basic
	PROPERTIES
		CXX_STANDARD 11
)

target_link_libraries(terml_test_basic
	PUBLIC
		terml
)