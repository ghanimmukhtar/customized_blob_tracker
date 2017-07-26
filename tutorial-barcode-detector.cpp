/*//! \example tutorial-barcode-detector.cpp
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
#ifdef VISP_HAVE_MODULE_SENSOR
#include <visp3/sensor/vpV4l2Grabber.h>
#endif

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
    vpImage<vpRGBa> Irgb(480, 640);
    vpDisplayX displayRgb;

    displayRgb.init(Irgb, 900, 200, "Color Image");

    // A click to stop acquisition
    std::cout << "Click in one image to stop acquisition" << std::endl;

    std::cout << "Trying to convert RGBa to Grey ..." << std::endl;
    vpImageConvert::RGBaToGrey((unsigned char *) Irgb.bitmap, I.bitmap, 307200);
    std::cout << "... RGBa to Grey is done ..." << std::endl;

    std::cout << "Trying to display grey image ..." << std::endl;

    vpDisplay::display(Irgb);
    vpDisplay::flush(Irgb);
    vpDisplayX d(I, 0, 0, "Camera view");

#if (VISP_HAVE_OPENCV_VERSION >= 0x020100) && (defined(VISP_HAVE_ZBAR) || defined(VISP_HAVE_DMTX))

  try {
    vpImage<unsigned char> I; // for gray images

    //! [Construct grabber]
#if defined(VISP_HAVE_V4L2)
    vpV4l2Grabber g;
    std::ostringstream device;
    device << "/dev/video" << opt_device;
    g.setDevice(device.str());
    g.setScale(1);
    g.acquire(I);
    //! [Construct grabber]

#if defined(VISP_HAVE_X11)
    vpDisplayX d(I);
    vpDisplay::setTitle(I, "ViSP viewer");

    vpDetectorBase *detector = NULL;
#if (defined(VISP_HAVE_ZBAR) && defined(VISP_HAVE_DMTX))
    if (opt_barcode == 0)
      detector = new vpDetectorQRCode;
    else
      detector = new vpDetectorDataMatrixCode;

    for(;;) {
      //! [Acquisition]
#if defined(VISP_HAVE_V4L2)
      g.acquire(I);
#endif
      //! [Acquisition]
      vpDisplay::display(I);

      bool status = detector->detect(I);
      std::ostringstream legend;
      legend << detector->getNbObjects() << " bar code detected";
      vpDisplay::displayText(I, 10, 10, legend.str(), vpColor::red);

      if (status) {
        for(size_t i=0; i < detector->getNbObjects(); i++) {
          std::vector<vpImagePoint> p = detector->getPolygon(i);
          vpRect bbox = detector->getBBox(i);
          vpDisplay::displayRectangle(I, bbox, vpColor::green);
          vpDisplay::displayText(I, (int)bbox.getTop()-20, (int)bbox.getLeft(), "Message: \"" + detector->getMessage(i) + "\"", vpColor::red);
          for(size_t j=0; j < p.size(); j++) {
            vpDisplay::displayCross(I, p[j], 14, vpColor::red, 3);
            std::ostringstream number;
            number << j;
            vpDisplay::displayText(I, p[j]+vpImagePoint(10,0), number.str(), vpColor::blue);
          }
        }
      }

      vpDisplay::displayText(I, (int)I.getHeight()-25, 10, "Click to quit...", vpColor::red);
      vpDisplay::flush(I);
      if (vpDisplay::getClick(I, false)) // a click to exit
        break;
    }
    delete detector;
  }
  catch(vpException &e) {
    std::cout << "Catch an exception: " << e << std::endl;
  }
#endif
}
*/


//! \example tutorial-barcode-detector-live.cpp
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

#ifdef VISP_HAVE_MODULE_SENSOR
#include <visp3/sensor/vpV4l2Grabber.h>
#endif

int main(int argc, const char** argv)
{
    Freenect::Freenect freenect;
    vpKinect & kinect = freenect.createDevice<vpKinect>(0);
    int opt_barcode = 0; // 0=QRCode, 1=DataMatrix
    try {
        // Set tilt angle in degrees
        if (0) {
            float angle = -3;
            kinect.setTiltDegrees(angle);
        }

        // Init display

        kinect.start(vpKinect::DMAP_MEDIUM_RES); // Start acquisition thread with a depth map resolution of 480x640
        vpImage<unsigned char> Idmap(480,640);//for medium resolution
        vpImage<float> dmap(480,640);//for medium resolution

        vpImage<vpRGBa> Irgb(480,640), Iwarped(480,640);


        vpDisplayX display, displayRgb, displayRgbWarped;


        display.init(Idmap, 100, 200,"Depth map");
        displayRgb.init(Irgb, 900, 200,"Color Image");
        displayRgbWarped.init(Iwarped,900,700,"Warped Color Image");

        // A click to stop acquisition
        std::cout << "Click in one image to stop acquisition" << std::endl;
        vpImage<unsigned char> I(480, 640); // for gray images


        kinect.getDepthMap(dmap);
        kinect.getDepthMap(dmap, Idmap);
        kinect.getRGB(Irgb);


        //Warped RGB image:
        kinect.warpRGBFrame(Irgb,dmap, Iwarped);
        vpDisplay::display(Iwarped);
        vpDisplay::flush(Iwarped);
        std::cout << "Trying to convert RGBa to Grey ..." << std::endl;
        vpImageConvert::RGBaToGrey((unsigned char *) Irgb.bitmap, I.bitmap, 307200);
        std::cout << "... RGBa to Grey is done ..." << std::endl;

        std::cout << "Trying to display grey image ..." << std::endl;

        vpDisplayX d(I);

        vpDisplay::setTitle(I, "ViSP viewer");
        std::cout << "... Grey is displayed ..." << std::endl;

        vpDetectorBase *detector = NULL;
        if (opt_barcode == 0)
            detector = new vpDetectorQRCode;
        else
            detector = new vpDetectorDataMatrixCode;


        for(;;) {
            //! [Acquisition]
            kinect.getDepthMap(dmap);
            kinect.getDepthMap(dmap, Idmap);
            kinect.getRGB(Irgb);
            vpImageConvert::RGBaToGrey((unsigned char *) Irgb.bitmap, I.bitmap, 307200);

            vpDisplay::display(Idmap);
            vpDisplay::flush(Idmap);
            vpDisplay::display(Irgb);
            vpDisplay::flush(Irgb);

            //Warped RGB image:
            kinect.warpRGBFrame(Irgb,dmap, Iwarped);
            vpDisplay::display(Iwarped);
            vpDisplay::flush(Iwarped);

            //! [Acquisition]
            vpDisplay::display(I);

            bool status = detector->detect(I);
            std::ostringstream legend;
            legend << detector->getNbObjects() << " bar code detected";
            vpDisplay::displayText(I, 10, 10, legend.str(), vpColor::red);

            if (status) {
                for(size_t i=0; i < detector->getNbObjects(); i++) {
                    std::vector<vpImagePoint> p = detector->getPolygon(i);
                    vpRect bbox = detector->getBBox(i);
                    vpDisplay::displayRectangle(I, bbox, vpColor::green);
                    vpDisplay::displayText(I, (int)bbox.getTop()-20, (int)bbox.getLeft(), "Message: \"" + detector->getMessage(i) + "\"", vpColor::red);
                    for(size_t j=0; j < p.size(); j++) {
                        vpDisplay::displayCross(I, p[j], 14, vpColor::red, 3);
                        std::ostringstream number;
                        number << j;
                        vpDisplay::displayText(I, p[j]+vpImagePoint(10,0), number.str(), vpColor::blue);
                    }
                }
            }

            vpDisplay::displayText(I, (int)I.getHeight()-25, 10, "Click to quit...", vpColor::red);
            vpDisplay::flush(I);
            if (vpDisplay::getClick(I, false)) // a click to exit
                break;
        }
        delete detector;
    }
    catch(vpException &e) {
        std::cout << "Catch an exception: " << e << std::endl;
    }

}
