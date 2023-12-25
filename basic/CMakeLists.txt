cmake_minimum_required(VERSION 3.18)

add_executable(basic
	main.cpp
)

set_target_properties(basic
	PROPERTIES
		CXX_STANDARD 11
)

target_link_libraries(basic
	PUBLIC
		terml
)