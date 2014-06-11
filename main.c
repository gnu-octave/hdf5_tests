#include <stdlib.h>
#include <hdf5.h>
#include <hdf5_hl.h>

int append_hdf5_image(const char *filename, const char * img_name, unsigned char *img, int width, int height)
{
  hid_t  file_id;
  herr_t status;
  file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT );
  status  = H5IMmake_image_24bit(file_id, img_name, width, height, "INTERLACE_PIXEL", img );
  H5Fclose( file_id );
  return status;
}

int main()
{
  char filename[] = "myfile.h5";
  hid_t       file_id, dataspace_id;  /* identifiers */

  /* create a new HDF5 file using default properties. */
  file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );

  //************** general *************************
  hid_t string_dataspace = H5Screate(H5S_SCALAR);
  hid_t atype = H5Tcopy (H5T_C_S1);
  H5Tset_size (atype, 100);
  H5Tset_strpad(atype,H5T_STR_NULLTERM);

  hid_t string_dataset_id = H5Dcreate2(file_id, "/prog_version", atype, string_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Dwrite(string_dataset_id, atype, H5S_ALL, H5S_ALL, H5P_DEFAULT, "h5test v0.1.0");
  H5Dclose(string_dataset_id);

  string_dataset_id = H5Dcreate2(file_id, "/comment", atype, string_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Dwrite(string_dataset_id, atype, H5S_ALL, H5S_ALL, H5P_DEFAULT, "hello world");
  H5Dclose(string_dataset_id);

  //************** save 5x3 array *************************
  hid_t       pattern_dataset_id;
  hsize_t     dims[2];
  /* Create the data space for the pattern. */
  dims[0] = 5;
  dims[1] = 3; //period, angle, phase
  dataspace_id = H5Screate_simple(2, dims, NULL);

  /* Create the dataset. */
  pattern_dataset_id = H5Dcreate2(file_id, "/pattern", H5T_IEEE_F64LE, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  double pattern_data[dims[0]][dims[1]];
  for (unsigned int i=0; i<dims[0]; ++i)
    {
      pattern_data[i][0] = i;
      pattern_data[i][1] = i*10.01;
      pattern_data[i][2] = i*100.01;
    }

  /* Write the dataset. */
  H5Dwrite(pattern_dataset_id, H5T_IEEE_F64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, pattern_data);

  //give attributes to /pattern
  hid_t attr1 = H5Acreate2(pattern_dataset_id, "author", atype, string_dataspace, H5P_DEFAULT, H5P_DEFAULT);
  H5Awrite(attr1, atype, "this pattern was created by h5test");
  H5Aclose(attr1);

  attr1 = H5Acreate2(pattern_dataset_id, "comment", atype, string_dataspace, H5P_DEFAULT, H5P_DEFAULT);
  H5Awrite(attr1, atype, "add a comment");
  H5Aclose(attr1);

  H5Sclose(string_dataspace);
  H5Tclose(atype);

  /* End access to the dataset and release resources used by it. */
  H5Dclose(pattern_dataset_id);

  /* Terminate access to the data space. */
  H5Sclose(dataspace_id);

  //**************** save int vector ********************
  dims[0] = 5;
  int samples[] = {1, 3, 5, 7, 11};

  hid_t cam_dataspace_id = H5Screate_simple(1, dims, NULL);
  hid_t cam_roi_dataset_id = H5Dcreate2(file_id, "/primes", H5T_STD_I32LE, cam_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Dwrite(cam_roi_dataset_id, H5T_STD_I32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, samples);
  H5Sclose(cam_dataspace_id);
  H5Dclose(cam_roi_dataset_id);

  //**************** save scalar double ********************
  double cam_exposure_time = 13.456789;
  hid_t cam_exposure_dataspace = H5Screate(H5S_SCALAR);
  hid_t cam_exposure_dataset_id = H5Dcreate2(file_id, "/exposure_time", H5T_IEEE_F64LE, cam_exposure_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Dwrite(cam_roi_dataset_id, H5T_IEEE_F64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &cam_exposure_time);
  H5Sclose(cam_exposure_dataspace);
  H5Dclose(cam_exposure_dataset_id);

  H5Fclose( file_id );

  /*************** create test image ***********************/

  /* global buffer for image data */
  hsize_t width = 256;
  hsize_t height = 256;
  int len = width * height * 3 * sizeof( unsigned char );
  unsigned char *gbuf = (unsigned char*) malloc (len);

  //create image1
  for(int n=0; n<len; n+=3)
    {
      gbuf[n]=n/3/width;      //red
      gbuf[n+1]=(n/3)%width;  //green
      gbuf[n+2]=0;            //blue
    }
  append_hdf5_image(filename, "img1", gbuf, width, height);

  for(int n=0; n<len; n+=3)
    {
      gbuf[n]=n/3/width;
      gbuf[n+1]=(n/3)%width;
      gbuf[n+2]=gbuf[n]/2 + gbuf[n+1]/2;
    }
  append_hdf5_image(filename, "img2", gbuf, width, height);

  free(gbuf);
  return EXIT_SUCCESS;
}






