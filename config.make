OF_ROOT = ../../..
USER_CFLAGS = -I/usr/local/include/opencv -I/usr/local/include -L/usr/local/lib
USER_LDFLAGS = 
USER_LIBS = -larmadillo -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -Os -O1
EXCLUDE_FROM_SOURCE="bin,.xcodeproj,obj,.git"
