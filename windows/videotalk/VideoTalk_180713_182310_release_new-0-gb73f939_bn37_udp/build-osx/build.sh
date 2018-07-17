cmake --build . --config Release || exit 1
macdeployqt ./cmake_temp/VideoTalk/Release/VideoTalk.app || exit 1