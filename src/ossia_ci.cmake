if (OSSIA_CI)
  if ( $ENV{TRAVIS} MATCHES true )
    message(STATUS "We're building on Travis, SHA: $ENV{TRAVIS_COMMIT}, TAG: $ENV{TRAVIS_TAG}")
    set(GIT_SHA $ENV{TRAVIS_COMMIT})
    string(SUBSTRING ${GIT_SHA} 0 7 GIT_SHA)
    set(GIT_SHA "$ENV{TRAVIS_TAG}-${GIT_SHA}")
  elseif ( $ENV{APPVEYOR} MATCHES True )
    set(GIT_SHA $ENV{APPVEYOR_REPO_COMMIT})
    string(SUBSTRING ${GIT_SHA} 0 7 GIT_SHA)
    set(GIT_DATE $ENV{APPVEYOR_REPO_COMMIT_TIMESTAMP})
    set(GIT_SHA "$ENV{APPVEYOR_REPO_TAG_NAME}-${GIT_SHA}")
    message(STATUS "We're building on Appveyor, SHA: $ENV{APPVEYOR_REPO_COMMIT}, last commit on $ENV{APPVEYOR_REPO_COMMIT_TIMESTAMP}")
  endif()
else()
  include(ossia_git_info)
  get_commit_sha(GIT_SHA)
  get_commit_date(GIT_DATE)
  get_tag(GIT_TAG)
endif()

message(STATUS "Build against git revision : ${GIT_SHA}, last commit on ${GIT_DATE}, tag: ${GIT_TAG}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_info.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/git_info.cpp" @ONLY)
target_sources(ossia PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/git_info.cpp" git_info.h)
