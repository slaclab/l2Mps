#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <vector>
#include <inttypes.h>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>
#include <stdexcept>

#include "l2Mps_blen.h"

int main(int argc, char const *argv[])
{
    const char *yamlDoc = "yaml/000TopLevel.yaml";
    const char *mpsRootName = "mmio/AmcCarrierCore/AppMps";

    Path root = IPath::loadYamlFile( yamlDoc, "NetIODev" );

    Path mpsRoot;
    try
    {
        mpsRoot = root->findByName(mpsRootName);

        
        std::cout << "BLEN for AMC0: BELN[0]" << std::endl;
        std::cout << "==============================================" << std::endl;
        MpsBlen MpsBlen0(MpsBlenFactory::create(mpsRoot, 0));
        std::cout << "==============================================" << std::endl;

        std::cout << "==========================" << std::endl;
        std::cout << "======== BLEN[0] =========" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << std::endl;

        uint8_t n;
        std::array<int,3> ch;

        try
        {
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            n = MpsBlen0->getThrCount(ch);
            std::cout << "Thr count = " << unsigned(n) << std::endl;
            std::cout << "Byte map  = " << unsigned(MpsBlen0->getByteMap(ch)) << std::endl;
        }
        catch (std::exception &e)
        {
            printf("Error channel info section: %s\n", e.what());
        }

        std::cout << "LCLS1 thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;

        try
        {
            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            ch[0] = 0;  // LCLS1 table
            ch[1] = 0;  // Min
            thrMin = MpsBlen0->getThreshold(ch);
            enMin = MpsBlen0->getThresholdEn(ch);
            ch[1] = 1;  // Max
            thrMax = MpsBlen0->getThreshold(ch);
            enMax = MpsBlen0->getThresholdEn(ch);

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
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            n = MpsBlen0->getThrCount(ch);
            std::cout << "Thr count = " << unsigned(n) << std::endl;
            std::cout << "Byte map  = " << unsigned(MpsBlen0->getByteMap(ch)) << std::endl;

            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            ch[0] = 1;  // IDLE table
            ch[1] = 0;  // Min
            thrMin = MpsBlen0->getThreshold(ch);
            enMin = MpsBlen0->getThresholdEn(ch);
            ch[1] = 1;  // Max
            thrMax = MpsBlen0->getThreshold(ch);
            enMax = MpsBlen0->getThresholdEn(ch);

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

        std::cout << "ALT thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;
        try
        {
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            ch[0] = 3;  // IDLE table
            for (int i = 0; i < n; ++i)
            {
                ch[2] = i;
                ch[1] = 0;  // Min
                thrMin[i] = MpsBlen0->getThreshold(ch);
                enMin[i] = MpsBlen0->getThresholdEn(ch);
                ch[1] = 1;  // Max
                thrMax[i] = MpsBlen0->getThreshold(ch);
                enMax[i] = MpsBlen0->getThresholdEn(ch);
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

        std::cout << "STD thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;
        try
        {
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            ch[0] = 2;  // STD table
            for (int i = 0; i < n; ++i)
            {
                ch[2] = i;
                ch[1] = 0;  // Min
                thrMin[i] = MpsBlen0->getThreshold(ch);
                enMin[i] = MpsBlen0->getThresholdEn(ch);
                ch[1] = 1;  // Max
                thrMax[i] = MpsBlen0->getThreshold(ch);
                enMax[i] = MpsBlen0->getThresholdEn(ch);
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


        std::cout << "BLEN for AMC1: BELN[1]" << std::endl;
        std::cout << "==============================================" << std::endl;
        MpsBlen MpsBlen1(MpsBlenFactory::create(mpsRoot, 1));
        std::cout << "==============================================" << std::endl;


        std::cout << "==========================" << std::endl;
        std::cout << "======== BLEN[1] =========" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << std::endl;

        try
        {
        
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            n = MpsBlen1->getThrCount(ch);
            std::cout << "Thr count = " << unsigned(n) << std::endl;
            std::cout << "Byte map  = " << unsigned(MpsBlen1->getByteMap(ch)) << std::endl;
        }
        catch (std::exception &e)
        {
            printf("Error channel info section: %s\n", e.what());
        }

        std::cout << "LCLS1 thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;

        try
        {
            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            ch[0] = 0;  // LCLS1 table
            ch[1] = 0;  // Min
            thrMin = MpsBlen1->getThreshold(ch);
            enMin = MpsBlen1->getThresholdEn(ch);
            ch[1] = 1;  // Max
            thrMax = MpsBlen1->getThreshold(ch);
            enMax = MpsBlen1->getThresholdEn(ch);

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
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            n = MpsBlen1->getThrCount(ch);
            std::cout << "Thr count = " << unsigned(n) << std::endl;
            std::cout << "Byte map  = " << unsigned(MpsBlen1->getByteMap(ch)) << std::endl;

            uint32_t thrMax, thrMin;
            bool enMax, enMin;
            ch[0] = 1;  // IDLE table
            ch[1] = 0;  // Min
            thrMin = MpsBlen1->getThreshold(ch);
            enMin = MpsBlen1->getThresholdEn(ch);
            ch[1] = 1;  // Max
            thrMax = MpsBlen1->getThreshold(ch);
            enMax = MpsBlen1->getThresholdEn(ch);

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

        std::cout << "ALT thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;
        try
        {
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            ch[0] = 3;  // IDLE table
            for (int i = 0; i < n; ++i)
            {
                ch[2] = i;
                ch[1] = 0;  // Min
                thrMin[i] = MpsBlen1->getThreshold(ch);
                enMin[i] = MpsBlen1->getThresholdEn(ch);
                ch[1] = 1;  // Max
                thrMax[i] = MpsBlen1->getThreshold(ch);
                enMax[i] = MpsBlen1->getThresholdEn(ch);
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

        std::cout << "STD thresholds:" << std::endl;
        std::cout << "=========================" << std::endl;
        try
        {
            ch[0] = 0;
            ch[1] = 0;
            ch[2] = 0;

            uint32_t* thrMax = new uint32_t[n];
            uint32_t* thrMin = new uint32_t[n];
            bool* enMax = new bool[n];
            bool* enMin = new bool[n];
            ch[0] = 2;  // STD table
            for (int i = 0; i < n; ++i)
            {
                ch[2] = i;
                ch[1] = 0;  // Min
                thrMin[i] = MpsBlen1->getThreshold(ch);
                enMin[i] = MpsBlen1->getThresholdEn(ch);
                ch[1] = 1;  // Max
                thrMax[i] = MpsBlen1->getThreshold(ch);
                enMax[i] = MpsBlen1->getThresholdEn(ch);
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
