#include <visp3/core/vpImage.h>
#include <visp3/sensor/vpKinect.h>
#include <visp3/core/vpTime.h>

#include <visp3/core/vpConfig.h>
#include <visp3/core/vpImageConvert.h>
#include <visp3/detection/vpDetectorDataMatrixCode.h>
#include <visp3/detection/vpDetectorQRCode.h>
#include <visp3/gui/vpDisplayGDI.h>
#include <visp3/gui/vpDisplayGTK.h>
#include <visp3/gui/vpDisplayOpenCV.h>
#include <visp3/gui/vpDisplayX.h>
#include <visp3/blob/vpDot2.h>
#include <visp3/blob/vpDot.h>

int main(int argc, const char** argv)
{
    Freenect::Freenect freenect;
    vpKinect & kinect = freenect.createDevice<vpKinect>(0);

    // Set tilt angle in degrees
    if (0) {
        float angle = -3;
        kinect.setTiltDegrees(angle);
    }
    vpImage<unsigned char> I(480, 640); // Create a gray level image container

    // Init display
    kinect.start(vpKinect::DMAP_MEDIUM_RES); // Start acquisition thread with a depth map resolution of 480x640
    vpImage<vpRGBa> Irgb(480,640);
    vpDisplayX displayRgb;

    displayRgb.init(Irgb, 900, 200,"Color Image");

    // A click to stop acquisition
    std::cout << "Click in one image to stop acquisition" << std::endl;

    kinect.getRGB(Irgb);



    std::cout << "Trying to convert RGBa to Grey ..." << std::endl;
    vpImageConvert::RGBaToGrey((unsigned char *) Irgb.bitmap, I.bitmap, 307200);
    std::cout << "... RGBa to Grey is done ..." << std::endl;

    std::cout << "Trying to display grey image ..." << std::endl;

    vpDisplay::display(Irgb);
    vpDisplay::flush(Irgb);
    vpDisplayX d(I, 0, 0, "Camera view");

    vpDot2 blob;
    vpDot blob2;
    //blob2.
    blob.setGraphics(true);
    blob.setGraphicsThickness(2);
    blob.setGrayLevelMin(150);
    std::cout << "the max gray level is: " << blob.getGrayLevelMax() << std::endl;
    std::cout << "the min gray level is: " << blob.getGrayLevelMin() << std::endl;

    vpImagePoint germ;
    bool init_done = false;
    std::cout << "Click!!!" << std::endl;
    while(1) {
        try {
            kinect.getRGB(Irgb);
            vpImageConvert::RGBaToGrey((unsigned char *) Irgb.bitmap, I.bitmap, 307200);
            vpDisplay::display(I);

            if (! init_done) {
                vpDisplay::displayText(I, vpImagePoint(10,10), "Click in the blob to initialize the tracker", vpColor::red);
                if (vpDisplay::getClick(I, germ, false)) {
                    blob.initTracking(I, germ);
                    init_done = true;
                }
            }
            else {
                blob.track(I);
            }
            vpDisplay::flush(I);
        }
        catch(...) {
            init_done = false;
            blob.initTracking(I, germ);
            vpDisplay::flush(I);
            //std::cout << "Lost the blob trying to find it ASAP !!!!!!!!!!!!" << std::endl;
        }
    }
}
