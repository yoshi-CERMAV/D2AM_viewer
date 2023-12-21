D2AM_viewer: window.cc my_file_dir.cc check_mask.cc 
	/usr/local/hdf5/bin/h5c++ -o D2AM_viewer window.cc check_mask.cc Savitzky_Golay_2d.cc /usr/local/lib/libfltk.a -lpthread -framework Cocoa -I./include  -std=c++11 -framework Accelerate -lfftw3

