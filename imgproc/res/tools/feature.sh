rm ../../res/f_faces/*
find ../../res/faces/ -name "*.bmp" |xargs -I {} ../../bin/Debug/imgproc  {}
mv ../../res/faces/*.feature ../../res/f_faces/
