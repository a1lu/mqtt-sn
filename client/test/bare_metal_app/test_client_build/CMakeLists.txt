set (BARE_METAL_PROTOCOL_OPTS "mqttsn::protocol::option::ClientIdStaticStorageSize<21>,mqttsn::protocol::option::GwAddStaticStorageSize<4>,mqttsn::protocol::option::TopicNameStaticStorageSize<128>,mqttsn::protocol::option::MessageDataStaticStorageSize<256>")

set (BARE_METAL_CLIENT_OPTS "mqttsn::client::option::ClientsAllocLimit<1>,mqttsn::client::option::TrackedGatewaysLimit<1>,mqttsn::client::option::RegisteredTopicsLimit<5>")

set (TEST_BARE_METAL_CLIENT_NAME "test_bare_metal")
set (MQTTSN_BARE_METAL_CLIENT_LIB_NAME "cc_mqttsn_${TEST_BARE_METAL_CLIENT_NAME}_client")

gen_lib_mqttsn_client("${TEST_BARE_METAL_CLIENT_NAME}" "${BARE_METAL_PROTOCOL_OPTS}" "${BARE_METAL_CLIENT_OPTS}" FALSE "-nostdlib")
target_compile_definitions("${MQTTSN_BARE_METAL_CLIENT_LIB_NAME}" PRIVATE "-DNOSTDLIB")

include_directories (
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/client/src/basic
    ${CMAKE_SOURCE_DIR}/client/src/basic)

#################################################################

function (bin_client_build)
    set (name "test_client_build")
    
    set (src 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/stub.cpp")

    set (link
        ${MQTTSN_BARE_METAL_CLIENT_LIB_NAME}
        ${STARTUP_LIB_NAME})

    add_executable(${name} ${src})
    target_link_libraries (${name} ${link})
    target_compile_definitions("${name}" PRIVATE "-DNOSTDLIB")
    link_app (${name})
endfunction ()

#################################################################

if (CMAKE_COMPILER_IS_GNUCC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib --std=c++11") 
endif ()

bin_client_build()
