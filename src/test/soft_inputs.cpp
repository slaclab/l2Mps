/**
 *-----------------------------------------------------------------------------
 * Title      : Common Platform MPS module test program
 * ----------------------------------------------------------------------------
 * File       : mps.cpp
 * Author     : Jesus Vasquez, jvasquez@slac.stanford.edu
 * Created    : 2020-04-24
 * ----------------------------------------------------------------------------
 * Description:
 * Application to set and read back all the Link Node Software Inputs.
 * ----------------------------------------------------------------------------
 * This file is part of l2Mps. It is subject to
 * the license terms in the LICENSE.txt file found in the top-level directory
 * of this distribution and at:
    * https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
 * No part of l2Mps, including this file, may be
 * copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include <signal.h>
#include <iostream>
#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <arpa/inet.h>
#include <assert.h>

#include "l2Mps_soft_inputs.h"
#include "helpers.h"

class Tester
{
public:
    Tester(Path root);

    void testInput(bool val, bool eVal, std::size_t index) const;
    void testAllInputs() const;

private:
    MpsSoftInputs mpsSoftInputs;
};

Tester::Tester(Path root)
:
    mpsSoftInputs(IMpsSoftInputs::create(root))
{
}

void Tester::testInput(bool val, bool eVal, std::size_t index) const
{
    std::cout << "Testing input " << index << ":" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Set value                = " << std::boolalpha << val << std::endl;
    std::cout << "Set error value          = " << std::boolalpha << eVal << std::endl;

    // Write values
    assert( true == mpsSoftInputs->setInput(val, index) );
    assert( true == mpsSoftInputs->setErrorInput(eVal, index) );

    // Read back values
    std::pair<bool, bool> valRB  { mpsSoftInputs->getInput(index) };
    assert( true == valRB.first);

    std::pair<bool, bool> eValRB { mpsSoftInputs->getErrorInput(index) };
    assert( true == eValRB.first);

    printPair( "Read back value          ", valRB );
    printPair( "Read back error value    ", eValRB );
    printPair( "Value word content       ", mpsSoftInputs->getInputWord(), true);
    printPair( "Error value word content ", mpsSoftInputs->getErrorInputWord(), true);

    // Assert read back values
    assert( valRB.second  == val);
    assert( eValRB.second == eVal);
}

void Tester::testAllInputs() const
{
    std::cout << "Testing all the inputs:" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << std::endl;

    std::size_t numInputs { mpsSoftInputs->getNumInputs() };

    for ( std::size_t i {0}; i < numInputs; ++i )
    {
        std::cout << "Testing input " << i << ":" << std::endl;
        std::cout << "----------------" << std::endl;

        for ( std::size_t j {0}; j < 2; ++j)
        {
            bool val  { !!j };  // Value
            bool eVal {  !j };  // Error value


            std::cout << "Set value                = " << std::boolalpha << val << std::endl;
            std::cout << "Set error value          = " << std::boolalpha << eVal << std::endl;
            mpsSoftInputs->setInput(val, i);
            mpsSoftInputs->setErrorInput(eVal, i);
            printPair( "Read back value          ", mpsSoftInputs->getInput(i) );
            printPair( "Read back error value    ", mpsSoftInputs->getErrorInput(i) );
            printPair( "Value word content       ", mpsSoftInputs->getInputWord(), true);
            printPair( "Error value word content ", mpsSoftInputs->getErrorInputWord(), true);
            std::cout << std::endl;
        }
    }

    std::cout << "Done!" << std::endl;
}

void usage(const char* name)
{
    std::cout << "Usage: " << name << "-a <IP_address> -Y <Yaml_top> [-i <App_ID>] [-e <Enable>] [-c] [-h]" << std::endl;
    std::cout << "    -h               : show this message." << std::endl;
    std::cout << "    -a <IP_address>  : IP address of the target FPGA." << std::endl;
    std::cout << "    -Y <Yaml_top>    : Path to YAML top level file." << std::endl;
    std::cout << "    -c <channel>     : Input channel to set (Needs -v and-f. Ignored if -A is used)." << std::endl;
    std::cout << "    -v <value>       : Input value to set (Needs -c. Ignored if -A is used)." << std::endl;
    std::cout << "    -e <error_value> : Error value to set (Needs -c. Ignored is -A is used)." << std::endl;
    std::cout << "    -A               : Set and read back all software inputs sequentially." << std::endl;
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    unsigned char buf[sizeof(struct in6_addr)];
    std::string ipAddr;
    std::string yamlDoc;
    int ch   {    -1 };
    int val  {    -1 };
    int eVal {    -1 };
    bool all { false };
    int c;

    while((c =  getopt(argc, argv, "a:Y:c:v:e:A")) != -1)
    {
        switch (c)
        {
            case 'a':
                if (!inet_pton(AF_INET, optarg, buf))
                {
                    std::cout << "Error: Invalid IP address." << std::endl;
                    exit(1);
                }
                ipAddr = std::string(optarg);
                break;
            case 'Y':
                yamlDoc = std::string(optarg);
                break;
            case 'c':
                if ( ( 1 != sscanf(optarg, "%d", &ch) ) || (ch < 0) || (ch> 15) )
                {
                    std::cout << "Error: Invalid channel number." << std::endl;
                    exit(1);
                }
                break;
            case 'v':
                if ( ( 1 != sscanf(optarg, "%d", &val) ) || ( (val != 0) && (val != 1) ) )
                {
                    std::cout << "Error: Invalid value." << std::endl;
                    exit(1);
                }
                break;
            case 'e':
                if ( ( 1 != sscanf(optarg, "%d", &eVal) ) || ( (eVal != 0) && (eVal != 1) ) )
                {
                    std::cout << "Error: Invalid error value." << std::endl;
                    exit(1);
                }
                break;
            case 'A':
                all = true;
                break;
            default:
                usage(argv[0]);
                exit(1);
                break;
        }
    }

    if (ipAddr.empty())
    {
        std::cout << "Error: Must specify an IP address." << std::endl;
        exit(1);
    }

    if (yamlDoc.empty())
    {
        std::cout << "Error: Must specify a YAML top path." << std::endl;
        exit(1);
    }

    if ( ( ! all ) && ( ( ch == -1 ) || ( val == -1 ) || ( eVal == -1 )) )
    {
        std::cout << "Error: must specify channel, value and error value; or use the '-A' option." << std::endl;
        exit(1);
    }

    IYamlSetIP setIP(ipAddr);
    Path root = IPath::loadYamlFile( yamlDoc.c_str(), "NetIODev", NULL, &setIP );

    std::cout << std::endl << std::endl;

    {
        Tester t ( root );

        if ( all )
        {
            t.testAllInputs();
        }
        else
        {
            t.testInput(!!val, !!eVal, ch);
        }
    }

    return 0;
}
