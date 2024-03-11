include(FetchContent)

# EXPERIMENTAL
FetchContent_Declare(
        quantiles
        GIT_REPOSITORY  https://github.com/Liam0205/stream-quantiles
        GIT_TAG         fe65a645e85d4f6f112dd232e527d52524024548
        GIT_SHALLOW     TRUE
)

FetchContent_GetProperties(quantiles)
FetchContent_Populate(quantiles)
