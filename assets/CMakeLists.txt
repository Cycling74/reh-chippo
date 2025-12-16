file(GLOB IMAGE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/images/*.png")
file(GLOB FONT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/fonts/*.ttf")

juce_add_binary_data(HopkinsBinaryData
    SOURCES
        ${IMAGE_FILES}
        ${FONT_FILES}
    HEADER_NAME
        "BinaryData.h"  # Optional: specify the header file name
    NAMESPACE
        BinaryData      # Optional: specify the namespace
)