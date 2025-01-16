
include(FetchContent)

FetchContent_Declare(
  pugi_xml
  GIT_REPOSITORY https://github.com/zeux/pugixml.git
  GIT_TAG v1.15  
)

FetchContent_MakeAvailable(pugi_xml)