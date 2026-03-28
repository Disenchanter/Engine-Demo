file(GLOB_RECURSE FW CONFIGURE_DEPENDS ./  *.cpp)

add_library(fw ${FW} )
