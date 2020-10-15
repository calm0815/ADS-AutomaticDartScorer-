# 変数SOURCE_CORDEを宣言し、opencv_testという値を入れる。
# cmake -D SOURCE_CODE=(ソース名)で上書き可
set(SOURCE_CODE image_updator CACHE NAME "Target object name")

# CMakeの最低バージョンを記述
cmake_minimum_required(VERSION 2.8)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 14) # C++14...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

# ソリューション名を指定
project( ${SOURCE_CODE} )

# OpenCVのパッケージを探す
find_package( OpenCV REQUIRED )

#ヘッダファイルのパスを指定
include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    )

# 実行ファイル名とソース指定（ここではソースと同じ名前の実行ファイルを作ります）
add_executable( ${SOURCE_CODE}
    src/${SOURCE_CODE}.cpp
    src/main.cpp
    )

#リンク先のライブラリを指定
target_link_libraries( ${SOURCE_CODE} ${OpenCV_LIBS} )