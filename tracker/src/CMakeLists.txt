
set(LOAD_IMAGES_LIB_DEPENDS
	boost_filesystem
	boost_system
	boost_regex
	${OpenCV_LIBRARIES}
	)

add_library(
	load_images 
	load_images.cpp
	)

set(TRACKER_LIB_DEPENDS
	${OpenCV_LIBRARIES}
	)

add_library(
	tracker 
	SRCDFTracker.cpp
	FeatureUtilities.cpp
	)

target_link_libraries(
	load_images
	${LOAD_IMAGES_LIB_DEPENDS}
	)

target_link_libraries(
	tracker
	${TRACKER_LIB_DEPENDS}
	)

add_executable(
	test_opencv 
	test_opencv.cpp
	)

target_link_libraries(
	test_opencv 
	load_images
	tracker
	)