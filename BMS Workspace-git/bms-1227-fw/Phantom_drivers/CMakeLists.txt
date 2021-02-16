cmake_minimum_required(VERSION 3.10)

get_filename_component(PROJECT_PARENT_DIR ../ ABSOLUTE)

project(soc)

add_executable(soc ${PROJECT_SOURCE_DIR}/source/soc.c ${PROJECT_SOURCE_DIR}/source/bms_data.c ${PROJECT_SOURCE_DIR}/unittest/soc_unittest.c)

target_include_directories(soc PUBLIC
							"${PROJECT_SOURCE_DIR}/include"
							"${PROJECT_PARENT_DIR}/halcogen_bms_master/include"
							"${PROJECT_SOURCE_DIR}/unittest/mock")

target_link_libraries(soc
						mock_halcogen_bms_master)
