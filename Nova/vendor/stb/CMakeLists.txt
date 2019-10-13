add_library(stb STATIC stb_image.h
								stb_perlin.h
								stb_build.cpp
						 )
target_include_directories(stb PRIVATE ${VENDOR_INCLUDE_DIRECTORIES})