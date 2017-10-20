#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <vector>
#include <inttypes.h>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>
#include <stdexcept>

#include "l2Mps_bpm.h"


int main(int argc, char const *argv[])
{
    const char *yamlDoc = "yaml/000TopLevel.yaml";
    const char *mpsRootName = "mmio/AmcCarrierCore/AppMps";

    Path root = IPath::loadYamlFile( yamlDoc, "NetIODev" );

    Path mpsRoot;
    try
    {
        mpsRoot = root->findByName(mpsRootName);

        std::cout << "BPM for AMC0: BPM[0]" << std::endl;
        std::cout << "==============================================" << std::endl;
        MpsBpm MpsBpm0(MpsBpmFactory::create(mpsRoot, 0));
        std::cout << "==============================================" << std::endl;

        std::cout << "BPM for AMC1: BPM[1]" << std::endl;
        std::cout << "==============================================" << std::endl;
        MpsBpm MpsBpm1(MpsBpmFactory::create(mpsRoot, 1));
        std::cout << "==============================================" << std::endl;

        std::cout << "=========================" << std::endl;
        std::cout << "======== BPM[0] =========" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << std::endl;

        uint8_t n;
        std::array<int,4> ch;

        try
        {
        // uint8_t n = MpsBpm0->ThrX()->getThrCount();
        ch[0] = 0;  // Chanel X
        ch[1] = 0;
        ch[2] = 0;
        ch[3] = 0;

        n = MpsBpm0->getThrCount(ch);
        std::cout << "Thr count = " << unsigned(n) << std::endl;
        // std::cout << "X, byte map =" << unsigned(MpsBpm0->ThrX()->getByteMap()) << std::endl;
        // std::cout << "Y, byte map =" << unsigned(MpsBpm0->ThrY()->getByteMap()) << std::endl;
        // std::cout << "C, byte map =" << unsigned(MpsBpm0->ThrC()->getByteMap()) << std::endl;
        std::cout << "X, byte map =" << unsigned(MpsBpm0->getByteMap(ch)) << std::endl;
        std::cout << "Y, byte map =" << unsigned(MpsBpm0->getByteMap(ch)) << std::endl;
        std::cout << "C, byte map =" << unsigned(MpsBpm0->getByteMap(ch)) << std::endl;

        std::cout << "LCLS1 thresholds:" << std::endl;
        printf("(%d, %d, %d, %d)\n", ch[0], ch[1], ch[2], ch[3]);
        std::cout << "=========================" << std::endl;

            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            // MpsBpm0->ThrX()->getLcls1Threshold(&enMin, &thrMin, &enMax, &thrMax);
            ch[1] = 0;  // LCLS1 table
            ch[2] = 0;  // Min
            thrMin = MpsBpm0->getThreshold(ch);
            enMin = MpsBpm0->getThresholdEn(ch);
            ch[2] = 1;  // Max
            thrMax = MpsBpm0->getThreshold(ch);
            enMax = MpsBpm0->getThresholdEn(ch);

            std::cout << "Min En = " << std::boolalpha << enMin << std::endl;
            std::cout << "Max En = " << std::boolalpha << enMax << std::endl;
            std::cout << "Min = " << unsigned(thrMin) << std::endl;
            std::cout << "Max = " << unsigned(thrMax) << std::endl;
            std::cout << std::endl;
        }
        catch (std::exception &e)
        {
            printf("Error on LCLS 1 table section: %s\n", e.what());
        }

        std::cout << "IDLE thresholds:" << std::endl;
        printf("(%d, %d, %d, %d)\n", ch[0], ch[1], ch[2], ch[3]);
        std::cout << "=========================" << std::endl;
        try
        {
            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            // MpsBpm0->ThrX()->getIdleThreshold(&enMin, &thrMin, &enMax, &thrMax);
            ch[1] = 1;  // IDLE table
            ch[2] = 0;  // Min
            thrMin = MpsBpm0->getThreshold(ch);
            enMin = MpsBpm0->getThresholdEn(ch);
            ch[2] = 1;  // Max
            thrMax = MpsBpm0->getThreshold(ch);
            enMax = MpsBpm0->getThresholdEn(ch);

            std::cout << "Min En = " << std::boolalpha << enMin << std::endl;
            std::cout << "Max En = " << std::boolalpha << enMax << std::endl;
            std::cout << "Min = " << unsigned(thrMin) << std::endl;
            std::cout << "Max = " << unsigned(thrMax) << std::endl;
            std::cout << std::endl;
            // ThrChannel1->setIdleThreshold(true, ch+2000, true, ch+4000);
        }
        catch (std::exception &e)
        {
            printf("Error on IDLE table section: %s\n", e.what());
        }

        std::cout << "ALT thresholds:" << std::endl;
        printf("(%d, %d, %d, %d)\n", ch[0], ch[1], ch[2], ch[3]);
        std::cout << "=========================" << std::endl;
        try
        {
            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            // MpsBpm0->ThrX()->getAltThreshold(enMin, thrMin, enMax, thrMax);
            ch[1] = 3;  // IDLE table
            for (int i = 0; i < n; ++i)
            {
                ch[3] = i;
                ch[2] = 0;  // Min
                thrMin[i] = MpsBpm0->getThreshold(ch);
                enMin[i] = MpsBpm0->getThresholdEn(ch);
                ch[2] = 1;  // Max
                thrMax[i] = MpsBpm0->getThreshold(ch);
                enMax[i] = MpsBpm0->getThresholdEn(ch);
            }

            std::cout << "Min EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMin[i] << ", ";
            std::cout << std::endl;
            std::cout << "Max EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMax[i] << ", ";
            std::cout << std::endl;
            std::cout << "Min = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMin[i]) << ", ";
            std::cout << std::endl;
            std::cout << "Max = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMax[i]) << ", ";
            std::cout << std::endl;
            std::cout << std::endl;
            delete[] thrMax, thrMin, enMax, enMin;
        }
        catch (std::exception &e)
        {
            printf("Error on Alt table section: %s\n", e.what());
        }

        try
        {
            std::cout << "STD thresholds:" << std::endl;
            printf("(%d, %d, %d, %d)\n", ch[0], ch[1], ch[2], ch[3]);
            std::cout << "=========================" << std::endl;
            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            // MpsBpm0->ThrX()->getStdThreshold(enMin, thrMin, enMax, thrMax);
            // ThrChannel1->getStdThreshold(enMin, thrMin, enMax, thrMax);
            ch[1] = 2;  // STD table
            for (int i = 0; i < n; ++i)
            {
                ch[3] = i;
                ch[2] = 0;  // Min
                thrMin[i] = MpsBpm0->getThreshold(ch);
                enMin[i] = MpsBpm0->getThresholdEn(ch);
                ch[2] = 1;  // Max
                thrMax[i] = MpsBpm0->getThreshold(ch);
                enMax[i] = MpsBpm0->getThresholdEn(ch);
            }

            std::cout << "Min EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMin[i] << ", ";
            std::cout << std::endl;
            std::cout << "Max EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMax[i] << ", ";
            std::cout << std::endl;
            std::cout << "Min = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMin[i]) << ", ";
            std::cout << std::endl;
            std::cout << "Max = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMax[i]) << ", ";
            std::cout << std::endl;
            std::cout << std::endl;
            delete[] thrMax, thrMin, enMax, enMin;
        }
        catch (std::exception &e)
        {
            printf("Error on STD table section: %s\n", e.what());
        }

        std::cout << "=========================" << std::endl;
        std::cout << "======== BPM[1] =========" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << std::endl;

        try
        {
        // n = MpsBpm1->ThrX()->getThrCount();
        ch[0] = 0;  // Chanel X
        ch[1] = 0;
        ch[2] = 0;
        ch[3] = 0;
        
        n = MpsBpm1->getThrCount(ch);
        
        std::cout << "Thr count = " << unsigned(n) << std::endl;

        std::cout << "LCLS1 thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;

            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            // MpsBpm1->ThrX()->getLcls1Threshold(&enMin, &thrMin, &enMax, &thrMax);
            ch[1] = 0;  // LCLS1 table
            ch[2] = 0;  // Min
            thrMin = MpsBpm1->getThreshold(ch);
            enMin = MpsBpm1->getThresholdEn(ch);
            ch[2] = 1;  // Max
            thrMax = MpsBpm1->getThreshold(ch);
            enMax = MpsBpm1->getThresholdEn(ch);

            std::cout << "Min En = " << std::boolalpha << enMin << std::endl;
            std::cout << "Max En = " << std::boolalpha << enMax << std::endl;
            std::cout << "Min = " << unsigned(thrMin) << std::endl;
            std::cout << "Max = " << unsigned(thrMax) << std::endl;
            std::cout << std::endl;
        }
        catch (std::exception &e)
        {
            printf("Error on LCLS 1 table section: %s\n", e.what());
        }

        std::cout << "IDLE thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;
        try
        {
            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            // MpsBpm1->ThrX()->getIdleThreshold(&enMin, &thrMin, &enMax, &thrMax);
            ch[1] = 1;  // IDLE table
            ch[2] = 0;  // Min
            thrMin = MpsBpm1->getThreshold(ch);
            enMin = MpsBpm1->getThresholdEn(ch);
            ch[2] = 1;  // Max
            thrMax = MpsBpm1->getThreshold(ch);
            enMax = MpsBpm1->getThresholdEn(ch);

            std::cout << "Min En = " << std::boolalpha << enMin << std::endl;
            std::cout << "Max En = " << std::boolalpha << enMax << std::endl;
            std::cout << "Min = " << unsigned(thrMin) << std::endl;
            std::cout << "Max = " << unsigned(thrMax) << std::endl;
            std::cout << std::endl;
            // ThrChannel1->setIdleThreshold(true, ch+2000, true, ch+4000);
        }
        catch (std::exception &e)
        {
            printf("Error on IDLE table section: %s\n", e.what());
        }

        std::cout << "ALT thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;
        try
        {
            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            // MpsBpm1->ThrX()->getAltThreshold(enMin, thrMin, enMax, thrMax);
            ch[1] = 3;  // ALT table
            for (int i = 0; i < n; ++i)
            {
                ch[3] = i;
                ch[2] = 0;  // Min
                thrMin[i] = MpsBpm1->getThreshold(ch);
                enMin[i] = MpsBpm1->getThresholdEn(ch);
                ch[2] = 1;  // Max
                thrMax[i] = MpsBpm1->getThreshold(ch);
                enMax[i] = MpsBpm1->getThresholdEn(ch);
            }

            std::cout << "Min EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMin[i] << ", ";
            std::cout << std::endl;
            std::cout << "Max EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMax[i] << ", ";
            std::cout << std::endl;
            std::cout << "Min = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMin[i]) << ", ";
            std::cout << std::endl;
            std::cout << "Max = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMax[i]) << ", ";
            std::cout << std::endl;
            std::cout << std::endl;
            delete[] thrMax, thrMin, enMax, enMin;
        }
        catch (std::exception &e)
        {
            printf("Error on Alt table section: %s\n", e.what());
        }

        try
        {
            std::cout << "STD thresholds:" << std::endl;
            std::cout << "=========================" << std::endl;
            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            ch[1] = 2;  // STD table
            for (int i = 0; i < n; ++i)
            {
                ch[3] = i;
                ch[2] = 0;  // Min
                thrMin[i] = MpsBpm1->getThreshold(ch);
                enMin[i] = MpsBpm1->getThresholdEn(ch);
                ch[2] = 1;  // Max
                thrMax[i] = MpsBpm1->getThreshold(ch);
                enMax[i] = MpsBpm1->getThresholdEn(ch);
            }

            std::cout << "Min EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMin[i] << ", ";
            std::cout << std::endl;
            std::cout << "Max EN = ";
            for (int i = 0; i<n; i++)
                std::cout << std::boolalpha << enMax[i] << ", ";
            std::cout << std::endl;
            std::cout << "Min = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMin[i]) << ", ";
            std::cout << std::endl;
            std::cout << "Max = ";
            for (int i = 0; i<n; i++)
                std::cout << unsigned(thrMax[i]) << ", ";
            std::cout << std::endl;
            std::cout << std::endl;
            delete[] thrMax, thrMin, enMax, enMin;
        }
        catch (std::exception &e)
        {
            printf("Error on STD table section: %s\n", e.what());
        }
    }
    catch (CPSWError &e)
    {
        printf("CPSW error: %s not found!\n", e.getInfo().c_str());
        return 1;
    }

    return 0;
}
