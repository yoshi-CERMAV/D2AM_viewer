//
//  YN_input.h
//  
//
//  Created by Yoshiharu Nishiyama on 20/11/2023.
//

#ifndef YN_input_h
#define YN_input_h
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Output.H>
#include <math.h>
#include <iostream>
using namespace std;
class YN_Float_Input: public Fl_Input
{
public:
    YN_Float_Input(int x, int y, int w, int h,const char * l = 0, double v=0, char const in_fmt[]=NULL ):
    Fl_Input(x, y, w, h, l){
        if(!in_fmt){strncpy(fmt, "%5.2f", 256);}
        else strncpy(fmt, in_fmt, 256);
        setv(v);
        tab_nav(1);
        when(FL_WHEN_ENTER_KEY);
    }
    int handle(int e) {
        int ret = Fl_Input::handle(e);
        if ( e == FL_KEYBOARD && Fl::event_key() == FL_Escape ) exit(0);
        if(active())
            switch (e) {
                case FL_KEYDOWN:
                    switch(Fl::event_key()){
                        case FL_Tab:
                        case FL_Enter:
                            ret = 1;
                            val = atof(value());
                            setv(val);
   //                         do_callback();
                            break;
                    }
            }
  //      if(!value_set) val = atof(value());
        return(ret);
    }

    void setv(double v){
        val = v;
        set();
        set_changed();
        do_callback();
    }
    void set(){
        char str_val[40];
        snprintf(str_val, 40, fmt, val);
        value(str_val);
    }
    double get(){return val;}
protected:
    double val;
    char fmt[256];
};

class YN_Input: public Fl_Input
{
public:
    YN_Input(int x, int y, int w, int h,const char * l = 0, double v=0, char const in_fmt[]=NULL ):
    Fl_Input(x, y, w, h, l){
        if(!in_fmt){strncpy(fmt, "%5.2f", 256);}
        else strncpy(fmt, in_fmt, 256);
        setv(v);
        tab_nav(1);
        when(FL_WHEN_ENTER_KEY);
        value_set =0;
    }
    
    void setv(double v){
        val = v;
        set();
        value_set =1;
        set_changed();
        do_callback();
    }
    void set(){
        char str_val[40];
        snprintf(str_val, 40, fmt, val);
        value(str_val);
    }
    
    int handle(int e) {
        int ret = Fl_Input::handle(e);
        if ( e == FL_KEYBOARD && Fl::event_key() == FL_Escape ) exit(0);
        if(active())
            switch (e) {
                case FL_KEYDOWN:
                    switch(Fl::event_key()){
                        case FL_Down:
                            val /=2;
                            setv(val);
                            ret =1;
    //                        do_callback();
                            break;
                        case FL_Up:
                            val *=2;
                            setv(val);
                            ret = 1;
  //                          do_callback();
                            break;
                        case FL_Enter:
                        case FL_Tab:
                            ret = 1;
                            val = atof(value());
                            setv(val);
  //                          do_callback();
                            break;
                    }
            }
        if(!value_set) val = atof(value());
        return(ret);
    }
    double get(){return val;}
protected:
    double val;
    int value_set ;
    char fmt[256];
};

class YN_Angle_Input: public Fl_Input
{
    const double rad2deg = 180/M_PI;
    const double deg2rad = M_PI/180;
public:
    YN_Angle_Input(int x, int y, int w, int h,const char * l = 0, double v=0, char const in_fmt[]=NULL ):
    Fl_Input(x, y, w, h, l){
        if(!in_fmt){strncpy(fmt, "%5.2f", 256);}
        else strncpy(fmt, in_fmt, 256);
        setv(v);
        tab_nav(1);
        when(FL_WHEN_ENTER_KEY);
        value_set =0;
    }
    void setv(double v){
        val = v;
        set();
        value_set =1;
        set_changed();
        do_callback();
    }
    void set(){
        char str_val[40];
        snprintf(str_val, 40, fmt, val*rad2deg);
        value(str_val);
    }
    
    int handle(int e) {
        int ret = Fl_Input::handle(e);
        if ( e == FL_KEYBOARD && Fl::event_key() == FL_Escape ) exit(0);
        if(active())
            switch (e) {
                case FL_KEYDOWN:
                    switch(Fl::event_key()){
                        case FL_Down:
                            val /=2;
                            setv(val);
                            ret =1;
 //                           do_callback();
                            break;
                        case FL_Up:
                            val *=2;
                            setv(val);
                            ret = 1;
//                            do_callback();
                            break;
                        case FL_Enter:
                        case FL_Tab:
                            ret = 1;
                            val = atof(value())*deg2rad;
                            setv(val);
 //                           do_callback();
                            break;
                    }
            }
        if(!value_set) val = atof(value())*deg2rad;
        return(ret);
    }
    double get(){return val;}
protected:
    double val;
    int value_set ;
    char fmt[256];
    
};


class YN_Int_Input: public Fl_Int_Input
{
public:
    YN_Int_Input(int x, int y, int w, int h,const char * l = 0, int v=0):
    Fl_Int_Input(x, y, w, h, l){
        set(v);
        tab_nav(1);
    }
    virtual void set(int i){
        ival = i;
        char val[10];
        snprintf(val, 10, "%d", ival);
        value(val);
    }
    virtual void increment(int i){
        ival += i;
        set(ival);
        do_callback();
    }
    virtual void decrement(int i){
        ival -= i;
        set(ival);
        do_callback();
    }
    int val(){return ival;}
    int handle(int e) {
        int ret = Fl_Input::handle(e);
        if(active()){
            switch (e) {
                case FL_KEYDOWN:
                    switch(Fl::event_key()){
                        case FL_Up:
                            increment(1);
                            break;
                        case FL_Down:
                            decrement(1);
                            break;
                        case FL_Enter:
                        case FL_Tab:
                            ret = 1;
                            ival = atoi(value());
//                            cout << value()<<endl;
                            do_callback();
                            break;

                    }
            }
        }
        return ret;
    }
protected:
    int ival;
};

class YN_Bound_Int_Input: public YN_Int_Input
{
public:
    YN_Bound_Int_Input(int x, int y, int w, int h,const char * l = 0, int v=0, int min = 0, int max = 100000):YN_Int_Input(x, y, w, h, l, v)
    {
        imin = min;
        imax = max;
    }
    void set_min(int i){imin = i;}
    void set_max(int i){imax = i;}
    
//    Fl_Int_Input(x, y, w, h, l){
//        set(v);
 //       tab_nav(1);
 //   }
    void set(int i){
        if(i > imax) i = imax;
        if(i< imin) i = imin;
        ival = i;
        char val[10];
        snprintf(val, 10, "%d", ival);
        value(val);
    }
    virtual void increment(int i){
        ival += i;
        if(ival> imax) ival = imax;
        cout <<ival<<endl;
        set(ival);
        do_callback();
    }
    virtual void decrement(int i){
        ival -= i;
        if(ival < imin) ival = imin;
        set(ival);
        do_callback();
    }

protected:
    int imin;
    int imax;
};

class YN_File_Num_Input: public YN_Bound_Int_Input
{
    
    static void set_step(Fl_Widget *w, void *data)
    {
        YN_File_Num_Input * input = (YN_File_Num_Input *)data;
        input->set_step(((YN_Bound_Int_Input *)(w))->val());
    }
public:
    YN_File_Num_Input(int x, int y, int w, int h,const char * l = 0, int v=0, int min = 0, int max = 100000):YN_Bound_Int_Input(x, y, w, h, l, v, min, max)
    {
        step = 1;
        step_input = new YN_Bound_Int_Input(x+w+30, y, w, h, "step", 1, 1, 25);
        step_input->callback(set_step, this);
    }
    void set_step(int i){step = i;}
    
    void set(int i){
        if(i > imax) i = imax;
        if(i < imin) i = imin;
        ival = i;
        char val[10];
        snprintf(val, 10, "%d", ival);
        value(val);
    }
    void increment(int i){
        ival += step;
        if(ival> imax) ival = imax;
        cout <<ival<<endl;
        set(ival);
        do_callback();
    }
    void decrement(int i){
        ival -= step;
        if(ival < imin) ival = imin;
        set(ival);
        do_callback();
    }

protected:
    int step;
    YN_Bound_Int_Input *step_input;
};
#endif /* YN_input_h */
