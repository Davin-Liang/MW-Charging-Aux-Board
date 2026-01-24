cmake_minimum_required(VERSION 3.15)

project(3rdparty_libs)

# for rknpu2
set(RKNN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rknpu2)
set(LIBRKNNRT ${RKNN_PATH}/lib/librknnrt.so PARENT_SCOPE)
set(LIBRKNNRT_INCLUDES ${RKNN_PATH}/include PARENT_SCOPE)
install(PROGRAMS ${RKNN_PATH}/lib/librknnrt.so DESTINATION lib)

# rga
set(RGA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/librga)
set(LIBRGA ${RGA_PATH}/lib/librga.a PARENT_SCOPE)
set(LIBRGA_INCLUDES ${RGA_PATH}/include PARENT_SCOPE)
install(PROGRAMS ${RGA_PATH}/lib/librga.so DESTINATION lib)

# stb image
set(STB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/stb_image)
set(STB_INCLUDES ${STB_PATH} PARENT_SCOPE)

# jpeg turbo
set(JPEG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/jpeg_turbo)
set(LIBJPEG ${JPEG_PATH}/lib/libturbojpeg.a PARENT_SCOPE)
set(LIBJPEG_INCLUDES ${JPEG_PATH}/include PARENT_SCOPE)

# libsndfile
set(LIBSNDFILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libsndfile)
set(LIBSNDFILE_INCLUDES ${LIBSNDFILE_PATH}/include PARENT_SCOPE)
set(LIBSNDFILE ${LIBSNDFILE_PATH}/lib/libsndfile.a PARENT_SCOPE)
