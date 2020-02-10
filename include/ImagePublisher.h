//
// Created by gedorinku on 2020/02/09.
//

#ifndef GF_ORB_SLAM2_IMAGEPUBLISHER_H
#define GF_ORB_SLAM2_IMAGEPUBLISHER_H


#include <string>
#include <boost/thread/thread.hpp>

template<class C>
class ImagePublisher
{
public:
    ImagePublisher(std::string sequenceDir, C callback) : thread(run, sequenceDir, callback)
    {
    }

private:
    boost::thread thread;

    static sensor_msgs::ImagePtr load(const std::string &filename)
    {
        cv::Mat image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
        if (image.empty())
        {
            std::cerr << "failed to load image: " << filename << std::endl;
        }
        std::cout << "loaded: " << filename << std::endl;
        return cv_bridge::CvImage(std_msgs::Header(), "bgr8", image).toImageMsg();
    }

    static void run(std::string sequenceDir, C callback)
    {
        std::ifstream fTimes;
        std::string strPathTimeFile = sequenceDir + "/times.txt";
        fTimes.open(strPathTimeFile.c_str());

        int imagesCount = 0;

        while (!fTimes.eof())
        {
            std::string s;
            getline(fTimes, s);
            if (!s.empty())
            {
                std::stringstream ss;
                ss << s;
                double t;
                ss >> t;
                imagesCount++;
            }
        }

        const auto prefixLeft = sequenceDir + "/image_0/";
        const auto prefixRight = sequenceDir + "/image_1/";

        // 1000でとめる
        imagesCount = 1000;
        for (int i = 0; i < imagesCount; i++)
        {
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(6) << i;
            const auto leftFilename = prefixLeft + ss.str() + ".png";
            const auto rightFilename = prefixRight + ss.str() + ".png";
            const auto leftImage = load(leftFilename);
            const auto rightImage = load(rightFilename);
            callback(leftImage, rightImage);
        }

        ros::shutdown();
    }

};


#endif //GF_ORB_SLAM2_IMAGEPUBLISHER_H
