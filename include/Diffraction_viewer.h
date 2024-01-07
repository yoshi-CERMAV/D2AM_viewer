//
//  Diffraction_viewer.h
//  
//
//  Created by Yoshiharu Nishiyama on 07/01/2024.
//

#ifndef Diffraction_viewer_h
#define Diffraction_viewer_h
Diffraction_viewer: public Fl_Widget{
public:
    Diffraction_viewer(int x, int y, int w, int h, const char *label = 0L): Fl_Widget(x, y, w, h, label){
        data = new uing8_t [w*h];
    }
    void draw_cross(int *ptr){
        fl_line(ptr[0]-10+x(), ptr[1]+y(), ptr[0]+10+x(), ptr[1]+y());
        fl_line(ptr[0]+x(), ptr[1]-10+y(), ptr[0]+x(), ptr[1]+10+y());
    }
    
    int handle(int e){
        int ret = Fl_Widget::handle(e);
        if(active()){
            switch(e){
                case FL_PUSH:
                    double x = Fl::event_x()-icx;
                    double y = Fl::event_y()-icy;
                    cout << x << " "<<y<<endl;
                    cursor_pos[0] = x;
                    cursor_pos[1] = y;
                    do_callback(this, cursor_pos);
                    break;
            }
        }
    }

    void set_center(double cx, double cy, double rad){
        icx = cx+x();
        icy = cy+y();
        radius = rad;
    }

    void draw(){
        fl_draw_image(data, x(), y(), w(),h(),1);
        fl_begin_line();
        fl_color(FL_GREEN);
        if(show_center_circle){
            fl_arc(icx, icy, radius, 0.0, 360.0);
        }
        if(show_cursor_pos){
            
        }
        fl_end_line();

    }
protected:
    bool show_center_circle = false;
    bool show_cursor_pos = false;

    double icx, icy, radius;
    double cursor_pos[2];
};

#endif /* Diffraction_viewer_h */
