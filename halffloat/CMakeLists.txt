add_library(halffloat STATIC
	include/half.h
	half.cpp
)

target_include_directories(halffloat PUBLIC include)
