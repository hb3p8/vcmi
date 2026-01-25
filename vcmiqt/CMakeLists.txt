set(vcmiqt_SRCS
		StdInc.cpp

		DebugStepInputProvider.cpp
		StepInputProvider.cpp
		StepInputProviderFactory.cpp
		jsonutils.cpp
		launcherdirs.cpp
)

set(vcmiqt_HEADERS
		StdInc.h

		DebugStepInputProvider.h
		MessageBox.h
		StepInputProvider.h
		StepInputProviderFactory.h
		jsonutils.h
		launcherdirs.h
		convpathqstring.h
		vcmiqt.h
)

if(IOS)
	list(APPEND vcmiqt_SRCS
		IosStepInputProvider.cpp
	)
	list(APPEND vcmiqt_HEADERS
		IosStepInputProvider.h
	)
endif()

assign_source_group(${vcmiqt_SRCS} ${vcmiqt_HEADERS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(ENABLE_STATIC_LIBS OR NOT (ENABLE_EDITOR AND ENABLE_LAUNCHER))
	add_library(vcmiqt STATIC ${vcmiqt_SRCS} ${vcmiqt_HEADERS})
	target_compile_definitions(vcmiqt PRIVATE VCMIQT_STATIC)
	set(TARGET_IS_SHARED OFF)
else()
	add_library(vcmiqt SHARED ${vcmiqt_SRCS} ${vcmiqt_HEADERS})
	target_compile_definitions(vcmiqt PRIVATE VCMIQT_SHARED)
	set(TARGET_IS_SHARED ON)
endif()

if(WIN32)
	set_target_properties(vcmiqt
		PROPERTIES
			OUTPUT_NAME "VCMI_vcmiqt"
			PROJECT_LABEL "VCMI_vcmiqt"
	)
endif()

target_link_libraries(vcmiqt vcmi Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

if(IOS)
	target_link_libraries(vcmiqt iOS_utils)
endif()

target_include_directories(vcmiqt PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

if(TARGET_IS_SHARED)
	install(TARGETS vcmiqt RUNTIME DESTINATION ${LIB_DIR} LIBRARY DESTINATION ${LIB_DIR})
endif()

vcmi_set_output_dir(vcmiqt "")
enable_pch(vcmiqt)
