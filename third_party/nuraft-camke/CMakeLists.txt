set(ROOT_SRC ${NURAFT_ROOT_DIR})

set(NURAFT_SRCS
    ${ROOT_SRC}/asio_service.cxx
    ${ROOT_SRC}/buffer.cxx
    ${ROOT_SRC}/buffer_serializer.cxx
    ${ROOT_SRC}/cluster_config.cxx
    ${ROOT_SRC}/crc32.cxx
    ${ROOT_SRC}/error_code.cxx
    ${ROOT_SRC}/global_mgr.cxx
    ${ROOT_SRC}/handle_append_entries.cxx
    ${ROOT_SRC}/handle_client_request.cxx
    ${ROOT_SRC}/handle_custom_notification.cxx
    ${ROOT_SRC}/handle_commit.cxx
    ${ROOT_SRC}/handle_join_leave.cxx
    ${ROOT_SRC}/handle_priority.cxx
    ${ROOT_SRC}/handle_snapshot_sync.cxx
    ${ROOT_SRC}/handle_timeout.cxx
    ${ROOT_SRC}/handle_user_cmd.cxx
    ${ROOT_SRC}/handle_vote.cxx
    ${ROOT_SRC}/launcher.cxx
    ${ROOT_SRC}/peer.cxx
    ${ROOT_SRC}/raft_server.cxx
    ${ROOT_SRC}/snapshot.cxx
    ${ROOT_SRC}/snapshot_sync_ctx.cxx
    ${ROOT_SRC}/snapshot_sync_req.cxx
    ${ROOT_SRC}/srv_config.cxx
    ${ROOT_SRC}/stat_mgr.cxx
)

add_library(_nuraft ${NURAFT_SRCS})

if (BOOST_INCLUDE_PATH AND BOOST_LIBRARY_PATH)
    # If Boost path (both include and library) is given,
    # use Boost's ASIO.
    message(STATUS "Boost include path: " ${BOOST_INCLUDE_PATH})
    message(STATUS "Boost library path: " ${BOOST_LIBRARY_PATH})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_ASIO")

    set(ASIO_INCLUDE_DIR ${BOOST_INCLUDE_PATH})
    set(LIBBOOST_SYSTEM "${BOOST_LIBRARY_PATH}/libboost_system.a")

endif()

if (OPENSSL_INCLUDE_PATH AND OPENSSL_LIBRARY_PATH)
    target_compile_definitions(_nuraft PRIVATE USE_BOOST_ASIO=1 BOOST_ASIO_STANDALONE=1)

    target_include_directories(_nuraft BEFORE PRIVATE "${OPENSSL_INCLUDE_PATH}")

    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_LIBRARY_PATH}/libcrypto.a")
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_LIBRARY_PATH}/libssl.a")
    
    target_link_libraries (_nuraft PRIVATE ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})

else()
    target_compile_definitions(_nuraft PRIVATE USE_BOOST_ASIO=1 BOOST_ASIO_STANDALONE=1 SSL_LIBRARY_NOT_FOUND=1)
endif()


target_include_directories(_nuraft BEFORE PRIVATE "${ASIO_INCLUDE_DIR}")
target_include_directories(_nuraft BEFORE PRIVATE "${OPENSSL_INCLUDE_PATH}") 
target_include_directories (_nuraft SYSTEM PRIVATE "${ROOT_SRC}/include/libnuraft")

target_link_libraries (_nuraft PRIVATE ${BOOST_INCLUDE_PATH})
`
add_library(levedb_contrib::nuraft ALIAS _nuraft)


