cmake_minimum_required(VERSION 3.18)

add_executable(rogue
	main.cpp
)

set_target_properties(rogue
	PROPERTIES
		CXX_STANDARD 11
)

target_link_libraries(rogue
	PUBLIC
		terml
)