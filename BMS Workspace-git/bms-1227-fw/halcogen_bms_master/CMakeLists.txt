cmake_minimum_required(VERSION 3.10)

project(mock_halcogen_bms_master)

add_library(mock_halcogen_bms_master ${PROJECT_SOURCE_DIR}/mock/mock_os_task.c)

target_include_directories(mock_halcogen_bms_master PUBLIC
							"${PROJECT_SOURCE_DIR}/include")