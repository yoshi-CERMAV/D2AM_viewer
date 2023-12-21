//
//  Read_D5WOS.h
//  D2AM2023
//
//  Created by yoshi on 13/09/2023.
//

#ifndef Read_D5WOS_h
#define Read_D5WOS_h
#include <hdf5.h>
#include <hdf5_hl.h>
#include <H5Cpp.h>
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
using namespace std;
using namespace H5;
#define SIZE_WOS 600 * 1156
#define SIZE_D5 578 * 960
#define YN_MAX_LEN 256

class read_h5
{
public:
    static herr_t
    file_count(hid_t loc_id, const char *name, const H5L_info_t *linfo, void *opdata)
    {
        read_h5 *reader = static_cast<read_h5 *>(opdata);
        reader->num_frame++;
        return 0;
    }

    read_h5(vector<string> file_names){
        strncpy(filename_, file_names[0].c_str(), YN_MAX_LEN);
        strncpy(D5fmt_,"/%d.1/measurement/D5", YN_MAX_LEN);
        strncpy(WOSfmt_,"/%d.1/measurement/WOS", YN_MAX_LEN);
        file_ = new H5File(filename_, H5F_ACC_RDONLY);
        num_frame = 0;
        herr_t idx = H5Literate(file_->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL,  file_count, this);
    }
    
    void open(const char filename[])
    {
        file_->close();
        strncpy(filename_, filename, YN_MAX_LEN);
        file_->openFile(filename_, H5F_ACC_RDONLY);
        num_frame = 0;
        herr_t idx = H5Literate(file_->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL,  file_count, this);

    }
    read_h5(const char filename[]){
        strncpy(filename_,filename, YN_MAX_LEN);
        strncpy(D5fmt_,"/%d.1/measurement/D5", YN_MAX_LEN);
        strncpy(WOSfmt_,"/%d.1/measurement/WOS", YN_MAX_LEN);
        file_ = new H5File(filename, H5F_ACC_RDONLY);
        num_frame = 0;
        cout << "going to count"<<endl;

        herr_t idx = H5Literate(file_->getId(), H5_INDEX_NAME, H5_ITER_INC, NULL,  file_count, this);
        cout << num_frame<<endl;
    }

    int set_path(const char path[])
    {
        struct stat sb;
        if(stat(path, &sb)) {cerr<<"directory does not exist"; return 0;}
        else strncpy(path_, path, YN_MAX_LEN);
        return 0;
    }
    int set_filename(const char filename[])
    {
        strncpy(filename_,filename, YN_MAX_LEN);
        struct stat buf;
        if(stat(filename, &buf)) {cerr<<"directory does not exist"; return 1;}
        if(!stat(filename, &buf)){
            if(file_) delete file_;
            file_ = new H5File(filename, H5F_ACC_RDONLY);
       }
        cout <<"fmt"<<D5fmt_<<endl;
        return 0;
    }
    
    
    int read_D5(int i, int *data){
        return read_data(i, data, D5fmt_, SIZE_D5);
    }
    int read_WOS(int i, int *data){
        return read_data(i, data, WOSfmt_, SIZE_WOS);
    }
    int read_data(int i, int *data, char fmt[], size_t max_size){
        cout << "reading"<<endl;
        char name [YN_MAX_LEN];
        cout <<"fmt"<< fmt<<" "<<D5fmt_<<endl;
        snprintf(name, YN_MAX_LEN, fmt, i);
        cout <<"name "<<name <<endl;
        DataSet dataset;
        cout << file_<<endl;
        struct stat buf;

        if (!file_){
            if(!stat(filename_, &buf)){
                file_ = new H5File(filename_, H5F_ACC_RDONLY);
                
            }else cerr << filename_ <<"does not exist"<<endl;
        }
        try {dataset = file_->openDataSet(name);}
        catch(FileIException &E){cerr <<"could not open dataspace";return 2;}
        if( !dataset.getId() )
            {
                cerr<<"ReportReaderHDF5: "
                     <<"Dataset " << name << " not found "
                     <<"in file: " << file_->getFileName();
                return -1;
            }
        cout <<"get Space"<<endl;
        DataSpace dataspace = dataset.getSpace();
        cout <<"space OK"<<endl;
        size_t required_size = dataset.getStorageSize();
        int rank = dataspace.getSimpleExtentNdims();
        cout <<"rank "<< rank;
        required_size =dataspace.getSimpleExtentNpoints();
        cout << "size "<<required_size<<endl;;
        cout << "required_size "<< required_size<<" "<< max_size<<endl;
        cout << required_size / max_size <<endl;
        if(required_size > max_size){cerr <<"not enough space "<<required_size  ; return 0;}
        if(!data) {cout<< "no memory "<<endl; return 1;}
        dataset.read(data, PredType::STD_I32LE, dataspace, dataspace);
        return 0;
    }
    ~read_h5(){file_->close();}
    int number_of_frames(){return num_frame;}
    char * filename(){return filename_;}
private:
    char path_[YN_MAX_LEN];
    char filename_[YN_MAX_LEN];
    char D5fmt_[YN_MAX_LEN];
    char WOSfmt_[YN_MAX_LEN];
    bool file_set = false;
    H5File *file_;
    int num_frame;
};

#endif /* Read_D5WOS_h */
