//
// Created by EA on 28.04.2016.
//

#ifndef OS3_PAGE_H
#define OS3_PAGE_H

#include "CommonDefines.h"

class Page {

public:

    Page(){
        for(int i = 0; i < PAGE_SIZE; ++i)
            frame[i] = 0;
    }

    int first() { return frame[0]; }


    int frame[PAGE_SIZE];
};


#endif //OS3_PAGE_H
