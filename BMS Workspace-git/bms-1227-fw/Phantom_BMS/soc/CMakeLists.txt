cmake_minimum_required(VERSION 3.10)

get_filename_component(PROJECT_PARENT_DIR ../ ABSOLUTE)
get_filename_component(PROJECT_GRANDPARENT_DIR ${PROJECT_PARENT_DIR}/.. ABSOLUTE)

project(soc)

add_executable(soc soc.c ${PROJECT_PARENT_DIR}/source/bms_data.c soc_unittest.c)

target_include_directories(soc PUBLIC
							"${PROJECT_PARENT_DIR}/include"
							"${PROJECT_GRANDPARENT_DIR}/halcogen_bms_master/include")

target_link_libraries(soc
						mock_halcogen_bms_master)