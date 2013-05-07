#
# Eigen 
#

include(ExternalProject)
include(FindPackageHandleStandardArgs)

set(EIGEN_URL http://bitbucket.org/eigen/eigen/get/3.1.1.tar.gz)

if(NOT TARGET eigen)
  ExternalProject_Add(eigen
    URL     http://dl.dropbox.com/u/782372/Software/eigen-eigen-43d9075b23ef.tar.gz
    URL_MD5 7f1de87d4bfef65d0c59f15f6697829d
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
               -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  )
endif()
ExternalProject_Get_Property(eigen INSTALL_DIR)

set(EIGEN_INCLUDE_DIR  ${INSTALL_DIR}/include/eigen3)
set(EIGEN_INCLUDE_DIRS ${EIGEN_INCLUDE_DIR})

find_package_handle_standard_args(EIGEN DEFAULT_MSG
  EIGEN_INCLUDE_DIR
)
