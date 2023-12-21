#include "D2AM_viewer.h"
#include <Savitzky_Golay_2d.h>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Output.H>
#include <unistd.h>
#include <detector.h>
#include "apply_poni2023.h"
#include <Read_D5WOS.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>

#define MAINWIN_W       880                             // main window w()
#define MAINWIN_H      600                             // main window h()
#define BROWSER_X       10                              // browser x()
#define BROWSER_Y       10                              // browser y()
#define BROWSER_W       100                             // browser w()
#define BROWSER_H       MAINWIN_H-35                    // browser h()
#define DATA_AREA_X      (BROWSER_W + 20)                // test area x()
#define DATA_AREA_Y      10                              // test area y()
#define DATA_AREA_W      600    // test area w()
#define DATA_AREA_H      430                       // test area h()
#define MASK_FILES_X

using namespace std;
const int image_size_WOS = 600 * 1156;
const int image_size_D5 = 578 * 960;
detector *WOS;
detector *D5;
int *dataWOS = 0;
int *dataD5 = 0;
int *mtWOS = 0;
int *mtD5 = 0;
char WOS_name[256];
char D5_name[256];
double scale = 1;
plot_data *plotdata_D5;
plot_data *plotdata_WOS;
int size_D5, size_WOS;


Fl_Output *cb_info=(Fl_Output *)0;

FILE *gp;
D2AM_viewer *viewer;
apply_poni *poniWOS = NULL;
apply_poni *poniD5 = NULL;
vector<string> file_name_list;
Fl_Input_Choice *D5_mask_choice;
Fl_Input_Choice *WOS_mask_choice;
Fl_Input_Choice *D5_flat_choice;
Fl_Input_Choice *WOS_flat_choice;
Fl_Input_Choice *D5_poni_choice;
Fl_Input_Choice *WOS_poni_choice;

YN_Float_Input *scale_input;
Filter2d *filter;


class YN_Filter: public Filter2d, public Fl_Group
{
    YN_Bound_Int_Input *width_input;
    YN_Bound_Int_Input *order_input;
public:
    YN_Filter(int x, int y, int w, int h,int sizex = 50, int sizey = 50, int nx = 5, int ny = 5, int ld = 0, int m =2):Filter2d(nx, ny, ld, m, sizex, sizey), Fl_Group(x, y, w, h)
    {
        int x0 = x + 100;
        int y0 = y+10;
        width_input = new YN_Bound_Int_Input(x0, y0, 100, 25, "width", nx, 1, 100);
        
        y0+=30;
        order_input = new YN_Bound_Int_Input(x0, y0, 100, 25, "order", m, 1, 5);
    }
    
};


const char * mylabel[] = {"D5 scale", "D5 max", "WOS scale", "WOS max","e"};
class D2AM_filename_Input;

class Pages
{
public:
    Pages();
protected:
    char label[256];
};


static void set_current_as_empty(Fl_Button *b, void *)
{
    cout <<" current is empty"<<endl;
    for(int i = 0; i < image_size_WOS; i++){
        mtWOS[i] = dataWOS[i];
    }
    cout << " WOS copied"<<endl;
    for(int i = 0; i < image_size_D5; i++){
        mtD5[i] = dataD5[i];
    }
    WOS->copy_to_bg();
    D5->copy_to_bg();
//    viewer->copy_data(mtD5, mtWOS);
}

static void get_scale(Fl_Button *b, void *)
{
    if(! poniD5) {cerr<<" need to install poni"<<endl;
        return ;
    }
    cout << " get scale"<<endl;
//    scale = poniD5->fit1(0.33, 0.36, dataD5, mtD5, D5->get_flat());
    scale = poniD5->fit(0.34, 0.35);
    scale_input->setv(scale);
}
static void scale_changed(Fl_Button *b, void *)
{
    if(! poniD5) {cerr<<" need to install poni"<<endl;
        return ;
    }
//    scale = poniD5->fit1(0.33, 0.36, dataD5, mtD5, D5->get_flat());
    scale = scale_input->get();
    cout <<" a "<<endl;
   // scale_input->setv(scale);
    cout <<" b "<<endl;

    viewer->show_diff(scale);
    cout <<" c "<<endl;

}


static void button_cb(Fl_Button *b, void *) {
  char msg[256];
  sprintf(msg, "Label: '%s'\nValue: %d", b->label(),b->value());
  cb_info->value(msg);
  cb_info->redraw();
  printf("%s\n",msg);
}

 void clicked_D5(Fl_Widget *w, long pos)
{
     cout <<pos <<" "<<D5->get_pos(pos)<<" ";
     int ipos =D5->get_pos(pos);
     D5->print_xy(ipos);
     poniD5->print_pos(ipos);
}

void clicked_WOS(Fl_Widget *w, long pos)
{
   poniD5->print_pos(WOS->get_pos(pos));
}


class D2AM_Window: public Fl_Double_Window{
public:
    D2AM_Window(int w, int h, const char *l=0L) :
      Fl_Double_Window(w, h, l),
      fTestAlignment(0)
    { }
    // this code is used by the viewport alignment test
    void drawAlignmentIndicators() {
      const int sze = 16;
      // top left corner
      fl_color(FL_GREEN); fl_yxline(0, sze, 0, sze);
      fl_color(FL_RED);   fl_yxline(-1, sze, -1, sze);
      fl_color(FL_WHITE); fl_rectf(3, 3, sze-2, sze-2);
      fl_color(FL_BLACK); fl_rect(3, 3, sze-2, sze-2);
      // bottom left corner
      fl_color(FL_GREEN); fl_yxline(0, h()-sze-1, h()-1, sze);
      fl_color(FL_RED);   fl_yxline(-1, h()-sze-1, h(), sze);
      fl_color(FL_WHITE); fl_rectf(3, h()-sze-1, sze-2, sze-2);
      fl_color(FL_BLACK); fl_rect(3, h()-sze-1, sze-2, sze-2);
      // bottom right corner
      fl_color(FL_GREEN); fl_yxline(w()-1, h()-sze-1, h()-1, w()-sze-1);
      fl_color(FL_RED);   fl_yxline(w(), h()-sze-1, h(), w()-sze-1);
      fl_color(FL_WHITE); fl_rectf(w()-sze-1, h()-sze-1, sze-2, sze-2);
      fl_color(FL_BLACK); fl_rect(w()-sze-1, h()-sze-1, sze-2, sze-2);
      // top right corner
      fl_color(FL_GREEN); fl_yxline(w()-1, sze, 0, w()-sze-1);
      fl_color(FL_RED);   fl_yxline(w(), sze, -1, w()-sze-1);
      fl_color(FL_WHITE); fl_rectf(w()-sze-1, 3, sze-2, sze-2);
      fl_color(FL_BLACK); fl_rect(w()-sze-1, 3, sze-2, sze-2);
    }
    void draw() {
      Fl_Double_Window::draw();
      if (fTestAlignment) {
        drawAlignmentIndicators();
      }
    }
    void testAlignment(int v) {
      fTestAlignment = v;
      redraw();
    }
    int fTestAlignment;
};

static void mask_select(Fl_Widget *b, void *det) {
    detector *d = (detector *)det;
    d->load_mask(((Fl_Input_Choice*)b)->value());
}
static void flat_select(Fl_Widget *b, void *det) {
    detector *d = (detector *)det;
    d->load_response(((Fl_Input_Choice*)b)->value());
}
static void poni_select(Fl_Widget *b, void *pon) {
    apply_poni *poni = (apply_poni *)pon;
    poni->read_poni(((Fl_Input_Choice*)b)->value());
}
static void show_diff(Fl_Widget *b, void *pon) {
    scale =scale_input->get();
    viewer->show_diff(scale);
}
static void integrate(Fl_Widget *b, void *pon) {
    fputs("plot x\n",gp);
    fflush(gp);

    poniWOS->integrate(0.45, 4, 0.01, mtWOS, dataWOS, scale, plotdata_WOS, size_WOS );
    poniD5->integrate(0.01, 0.625, 0.005, mtD5, dataD5, scale, plotdata_D5, size_D5);
    cout << size_WOS <<" "<< size_D5<<endl;
    fputs("set log x\n", gp);
    fputs("set log y\n", gp);
    fputs("plot \"-\" binary format =\"%float%float%float%int\" record=",gp);
    fprintf(gp, "%d", size_WOS);
    fputs(",\"-\" binary format =\"%float%float%float%int\" record=",gp);
    fprintf(gp, "%d\n", size_D5);
    fwrite(plotdata_WOS, size_WOS, 16, gp);
    fwrite(plotdata_D5, size_D5, 16, gp);
    fflush(gp);
//    void integrate(double qmin, double qmax, double qstep,
  //                 int *bg, int *dat, double scale, char filename[])
    
}

static void save(char fmt[], int num, plot_data *dat, int size)
{
    char filename [256];
    snprintf(filename, 256, fmt, num);
    ofstream fo(filename);
    fo.write(reinterpret_cast<char*>(dat), size*16);
}

static void save_data(Fl_Widget *b, void *pon) {
    save("%dD5.dat", viewer->file_num(), plotdata_D5, size_D5);
    save("%dWOS.dat", viewer->file_num(), plotdata_WOS, size_WOS);
}


void Set_File_Selector(int x, int y, int w, int h, char label[], const char name[],
                       Fl_Widget **c, Fl_Callback *call, void *d)
{
    (*c) = new Fl_Input_Choice(x, y, w, h, label);
    (*c)->callback(call, d);
    ifstream fi(name);
    char filename[256];
    while(fi.getline(filename, 256)){
        ((Fl_Input_Choice *)(*c))->add(filename);
    }
    ((Fl_Input_Choice *)(*c))->value(0);
    (*c)->do_callback();
}

int main(int argc, char *argv[])
{
//    Fl::option(Fl::OPTION_ARROW_FOCUS, 0);		// disable arrow focus nav (we want arrows to control cells)
    gp = popen("/usr/local/bin/gnuplot -persist", "w");
    ifstream fi(argv[1]);
    fi.getline(WOS_name, 256);
    fi.getline(D5_name, 256);
    char buffer[256];
    while(fi.getline(buffer,256)){
        file_name_list.push_back(string(buffer));
    }
    WOS = new detector (WOS_name); ///Users/yoshi/analysis/D2AM_base_files/WOS-NewServ_5Nov22.h5");
    D5 = new detector(D5_name);//"/Users/yoshi/analysis/D2AM_base_files/D5-NewServ_5Nov22.h5");
    poniWOS = new apply_poni(WOS);
    poniD5 = new apply_poni(D5);
    dataWOS = new int[image_size_WOS];
    dataD5 = new int[image_size_D5];
    mtWOS = new int[image_size_WOS];
    mtD5 = new int[image_size_D5];
    plotdata_D5 = new plot_data[10000];
    plotdata_WOS = new plot_data[10000];

    D5->init_quick_view(200, 400);
    WOS->init_quick_view(400, 200);
    D2AM_Window window(MAINWIN_W, MAINWIN_H, "D2AM viewer");
    window.begin();
    viewer =  new D2AM_viewer(DATA_AREA_X, DATA_AREA_Y, DATA_AREA_W, DATA_AREA_H+30, WOS, D5, dataWOS, dataD5,file_name_list);
    viewer->set_bg(mtD5, mtWOS);
    {
    int x =DATA_AREA_X;
    int y =DATA_AREA_Y+DATA_AREA_H+10;
    int y0 =y;
    Set_File_Selector(x, y, 200, 20, "D5_mask", "D5_mask",
                      (Fl_Widget **)&D5_mask_choice, mask_select, D5);
    y+=30;
    Set_File_Selector(x, y, 200, 20, "D5_flat", "D5_flat",
                      (Fl_Widget **)&D5_flat_choice, flat_select, D5);
    y+=30;
    Set_File_Selector(x, y, 200, 20, "D5_poni", "D5_poni",
                          (Fl_Widget **)&D5_poni_choice, poni_select, poniD5);
        y = y0;
        x+=300;
    Set_File_Selector(x, y, 200, 20, "WOS_mask", "WOS_mask",
                      (Fl_Widget **)&WOS_mask_choice, mask_select, WOS);
        y += 30;
    Set_File_Selector(x, y, 200, 20, "WOS_flat", "WOS_flat",
                      (Fl_Widget **)&WOS_flat_choice, flat_select, WOS);
        y += 30;
        Set_File_Selector(x, y, 200, 20, "WOS_poni", "WOS_poni",
                          (Fl_Widget **)&WOS_poni_choice, poni_select, poniWOS);

    }
    int x1 = 800;
    int y = 30;
    Fl_Button *b = new Fl_Button(x1, y, 70, 20, "set MT");
    b->callback((Fl_Callback*) set_current_as_empty);
    y+=30;
    Fl_Button *b1 = new Fl_Button(x1, y, 70, 20, "scale");
    b1->callback((Fl_Callback*) get_scale);    //    viewer->hide();
    y+=30;
    scale_input =  new YN_Float_Input (x1, y, 70, 20, "scale", 1.0, "%5.4f" );
    y+=30;
    scale_input->callback((Fl_Callback*) scale_changed);
    Fl_Button *b2 = new Fl_Button(x1, y, 70, 20, "diff");
    b2->callback((Fl_Callback*) show_diff);    //    viewer->hide();
    y+=30;
    Fl_Button *b3 = new Fl_Button(x1, y, 70, 20, "integrate");
    b3->callback((Fl_Callback*) integrate);
    y+=30;
    Fl_Button *b4 = new Fl_Button(x1, y, 70, 20, "save data");
    b4->callback((Fl_Callback*) save_data);

    window.end();

    window.show(argc, argv);
    return Fl::run();
}
