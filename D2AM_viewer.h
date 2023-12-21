#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/FL_Image.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Input_Choice.H>
#include <YN_input.h>
#include <unistd.h>
#include "apply_poni2023.h"
#include <Read_D5WOS.h>
using namespace std;
#define MAINWIN_W       700                             // main window w()
#define MAINWIN_H       400                             // main window h()
#define BROWSER_X       10                              // browser x()
#define BROWSER_Y       10                              // browser y()
#define BROWSER_W       100                             // browser w()
#define BROWSER_H       MAINWIN_H-35                    // browser h()
#define DATA_AREA_X      (BROWSER_W + 20)                // test area x()
#define DATA_AREA_Y      10                              // test area y()
#define DATA_AREA_W      (MAINWIN_W - BROWSER_W - 30)    // test area w()
#define DATA_AREA_H      BROWSER_H                       // test area h()
#define WOS_SIZE  600 * 1156
#define D5_SIZE  578 * 960
 void clicked_D5(Fl_Widget *w, long pos);
 void clicked_WOS(Fl_Widget *w, long pos);
class D2AM_view:public Fl_Widget{
public:
    D2AM_view(int x, int y, int w, int h, const char *label = 0L):Fl_Widget(x, y, w, h, label){
        data = new uint8_t [w*h];
    }
    ~D2AM_view(){if(data) delete []data;}
    unsigned char* image_ptr(){return data;}
    void draw(){
        fl_draw_image(data, x(), y(), w(), h(), 1);
    }
    int handle(int e){
        int ret = Fl_Widget::handle(e);
        if(active()){
            switch(e){
                case FL_PUSH:
                    cout <<Fl::event_x()-x() << " "<<Fl::event_y()-y()<<endl;
                    int pos = w() * (Fl::event_y()-y()) + Fl::event_x()-x();
                    
//                    cout <<Fl::event_x()<<" "<<Fl::event_y()<<endl;
                    do_callback(this, pos);
                    break;
                    
            }
        }
    }
 //   void win2img
protected:
    int pos;
    uint8_t *data;
};


class D2AM_viewer : public Fl_Group{
    static void D5_MaxMin_changed(Fl_Widget *w, void *data){
        D2AM_viewer * viewer = (D2AM_viewer *)data;
        viewer->D5_min_max_changed();
//        viewer->D5_scale_input->setv((viewer->D5_Max_input->get() - viewer->D5_Min_input->get())/256);
 //       if(showDiff){ show_diff_D5(); return;}
//        viewer->D5->make_image1(viewer->D5view->image_ptr(),viewer->dataD5, viewer->D5_Min_input->get(), viewer->D5_Max_input->get());
//        viewer->D5view->redraw();
    }
    static void WOS_MaxMin_changed(Fl_Widget *w, void *data)
    {
        D2AM_viewer * viewer = (D2AM_viewer *)data;
 //       if(showDiff){ show_diff_WOS(); return;}
        viewer->WOS_min_max_changed();
    }
    
    static void file_num_changed(Fl_Widget *w, void *data)
    {
  //      showDiff=false;
        D2AM_viewer * viewer = (D2AM_viewer *)data;
        viewer->file_num_changed2();
    }
    
    static void input_choice_cb(Fl_Widget *w, void *data)
    {
  //      showDiff=false;
        D2AM_viewer * viewer = (D2AM_viewer *)data;
        viewer->file_changed();
    }

public:
    detector *WOS;
    detector *D5;
    int *dataWOS = 0;
    int *dataD5 = 0;
    int *bgWOS = 0;
    int *bgD5 = 0;
    float cof;
    bool showDiff = false;
 //   D2AM_filename_Input *file_finder;
    YN_File_Num_Input *filenum_input;
    YN_Input *D5_Max_input;
    YN_Input *WOS_Max_input;
    YN_Input *D5_Min_input;
    YN_Input *WOS_Min_input;
    YN_Input *D5_scale_input;
    YN_Input *WOS_scale_input;
    Fl_Input_Choice *file_choser;
    D2AM_view *D5view;
    D2AM_view *WOSview;
    vector<string> filenames_;
    Fl_Output *cb_info=(Fl_Output *)0;
    read_h5 *reader ;
    
    void D5_min_max_changed(){
        double scale =(D5_Max_input->get()-D5_Min_input->get())/256;
        D5_scale_input->setv(scale);
        if(showDiff){ show_diff_D5(); return; }
        D5->make_image1(D5view->image_ptr(),dataD5, D5_Min_input->get(),
                         D5_Max_input->get());
        D5view->redraw();
    }

    void WOS_min_max_changed(){
        double scale =(WOS_Max_input->get()-WOS_Min_input->get())/256;
        WOS_scale_input->setv(scale);
        if(showDiff){ show_diff_WOS(); return ;}
        WOS->make_image1(WOSview->image_ptr(),dataWOS, WOS_Min_input->get(),
                         WOS_Max_input->get());
        WOSview->redraw();
    }
    
    void copy_data(int *out_D5, int *out_WOS)
    {
        for(int i = 0; i < D5_SIZE; i++){
            out_D5[i] = dataD5[i];
        }
        for(int i = 0; i < WOS_SIZE; i++){
            out_WOS[i] = dataWOS[i];
        }
 //       memcpy(out_D5, dataD5, sizeof(int)*D5_SIZE);
 //       memcpy(out_WOS, dataWOS, sizeof(int)*WOS_SIZE);
    }
    void file_num_changed2()
    {
        showDiff=false;
        int filenum  = filenum_input->val();
        cout << "filenum"<<filenum<<endl;
 //       read_h5 *reader =file_finder->reader();
        cout << reader <<" "<<reader->filename()<<endl;
        if(!reader) {cout <<"reader does not exist"<<endl; return;}
        reader->read_D5(filenum, dataD5);
        reader->read_WOS(filenum, dataWOS);
        D5->prepare(dataD5);
        WOS->prepare(dataWOS);
        cout << "reading "<<endl;
        double min =D5_Min_input->get();
        double max =D5_Max_input->get();
        cout << "D5 "<<min << " "<<max<<endl;
        D5->make_image1(D5view->image_ptr(),dataD5, min, max);
        min =WOS_Min_input->get();
        max =WOS_Max_input->get();
        cout << "WOS "<<min << " "<<max<<endl;
        
        WOS->make_image1(WOSview->image_ptr(),dataWOS, min, max);
        D5view->redraw();
        WOSview->redraw();
    }
    void show_diff(float in_cof)
    {
        cof = in_cof;
        showDiff = true;
        show_diff();
    }
    void show_diff()
    {
        show_diff_D5();
        show_diff_WOS();
    }
    void show_diff_D5(){
        double min =D5_Min_input->get();
        double max =D5_Max_input->get();
        D5->make_image1(D5view->image_ptr(),dataD5, bgD5, cof, min, max);
        D5view->redraw();
    }
    void show_diff_WOS(){
        double min =WOS_Min_input->get();
        double max =WOS_Max_input->get();
        WOS->make_image1(WOSview->image_ptr(),dataWOS,bgWOS, cof, min, max);
        WOSview->redraw();
    }
    void file_changed()
    {
        showDiff=false;
        reader->open(file_choser->value());
        int num_frame = reader->number_of_frames();
        cout << num_frame<<endl;
        filenum_input->set_max(num_frame+1);
        filenum_input->set_min(1);
        file_num_changed2();
    }
    
    int file_num(){return filenum_input->val();}
    
    void set_bg(int *D5_bg_in, int *WOS_bg_in){
        bgD5 =D5_bg_in;
        bgWOS =WOS_bg_in;
    }
public:
    D2AM_viewer(int X, int Y, int W, int H, detector *WOS_in, detector *D5_in, int *dataWOS_in, int *dataD5_in , vector<string> &filenames) : Fl_Group(X,Y,W,H){
        filenames_= filenames;
        WOS = WOS_in;
        D5 = D5_in;
        dataWOS =dataWOS_in;
        dataD5 = dataD5_in;
        //      dataWOS = new int [WOS_SIZE];
        //      dataD5 = new int [D5_SIZE];
        cout <<"starting "<<endl;
        begin();
        D5view = new D2AM_view(X,10,200,400);
        D5view->callback((Fl_Callback1*) clicked_D5);
        cout <<"D5VIew  "<<endl;
        WOSview = new D2AM_view (X+210,10,400,200);
        WOSview->callback((Fl_Callback1 *)clicked_WOS);
        cout <<"WOSCiew"<<endl;
        int view_bottom = WOSview->y()+WOSview->h();
        int X1 =DATA_AREA_X+280;
        int X2 =DATA_AREA_Y+600;
        D5_Max_input =  new YN_Input(X1 , view_bottom+10,100,25, "d5max", 256);
        D5_Min_input =  new YN_Input(X1, view_bottom+40,100,25, "d5min", 0);
        WOS_Max_input =  new YN_Input(X1, view_bottom+70,100,25, "WOSmax", 256);
        WOS_Min_input =  new YN_Input(X1, view_bottom+100,100,25, "WOSmin", 0);
        filenum_input = new YN_File_Num_Input(X1,view_bottom+130,100,25, "filenum",1);
 //       grp2->end();

        D5_scale_input = new YN_Input(X2, view_bottom+10,100,25, "d5scale", 1);
        WOS_scale_input = new YN_Input(X2, view_bottom+70,100,25, "WOSscale", 1);
        D5_scale_input->tab_nav(0);
        file_choser = new Fl_Input_Choice(D5view->x(),D5view->h()+D5view->y()+10,200,25,"Choose file");
        file_choser->callback(input_choice_cb, this);
        for(vector<string>::iterator itr = filenames.begin(); itr != filenames.end(); itr++){ file_choser->add(itr->c_str());
        }
        reader = new read_h5(filenames_[0].c_str());
        int num_frame = reader->number_of_frames();
        filenum_input->set_max(num_frame+1);
        filenum_input->set_min(1);
 //       file_finder = new D2AM_filename_Input(D5view->x(),D5view->h()+D5view->y()+10,600,25,"filename",  "/Users/yoshi/esrf/bm02/20230719/RAW_DATA/Yu/Yu_0001/Yu_0001.h5");
        D5_Max_input->when(FL_WHEN_CHANGED||FL_WHEN_ENTER_KEY);
        D5_Max_input->callback(D5_MaxMin_changed, this);
        D5_Min_input->callback(D5_MaxMin_changed, this);
        WOS_Max_input->callback(WOS_MaxMin_changed, this);
        WOS_Min_input->callback(WOS_MaxMin_changed, this);
        cout << "callbacks"<<endl;
        cout << "input "<<endl;
        filenum_input->value("1");
        filenum_input->callback(file_num_changed, this);
        file_num_changed2();
        end();
        show();
    }
};
