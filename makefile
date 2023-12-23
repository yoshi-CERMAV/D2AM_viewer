D2AM_viewer: window.cc my_file_dir.cc check_mask.cc 
	h5c++ -o D2AM_viewer window.cc check_mask.cc Savitzky_Golay_2d.cc -lfltk -lpthread  -I./include -I/usr/local/include -std=c++11  -lfftw3 -llapack -lcblas -lX11 -lXft -lfontconfig -lXrender -lsupc++ -lXdmcp -lXext
