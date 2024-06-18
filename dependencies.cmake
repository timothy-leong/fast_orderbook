include(FetchContent)

FetchContent_Declare(
  phmap
  GIT_REPOSITORY https://github.com/greg7mdp/parallel-hashmap.git
  GIT_TAG v1.3.12
)

FetchContent_MakeAvailable(phmap)