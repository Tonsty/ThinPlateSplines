file(GLOB SRCS *.c *.h)

# profiling
add_executable(tps-prof ${SRCS})
target_link_libraries(tps-prof tps)
install(TARGETS tps-prof RUNTIME DESTINATION bin)
