cmake_minimum_required(VERSION 3.10)

get_filename_component(PROJECT_PARENT_DIR ../ ABSOLUTE)

project(Phantom_drivers)

add_executable(phantom_pl455 ${PROJECT_SOURCE_DIR}/source/phantom_pl455.c phantom_pl455_unittest.c
				${PROJECT_SOURCE_DIR}/mock/mock_Phantom_sci.c ${PROJECT_SOURCE_DIR}/mock/mock_pl455.c)

target_include_directories(phantom_pl455 PUBLIC "${PROJECT_SOURCE_DIR}/include" 
							"${PROJECT_PARENT_DIR}/halcogen_bms_master/include")

target_link_libraries(phantom_pl455
						mock_halcogen_bms_master)

add_subdirectory(soc)