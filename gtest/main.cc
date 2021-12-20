//
// Created by wxm on 2020/11/26.
//

#include <iostream>
#include "gtest/gtest.h"

int main(int argc, char **argv)
{
    std::cout << "hello gtest" << std::endl;
    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}