//
// Created by EA on 28.04.2016.
//

#ifndef OS3_PAGETABLEENTRY_H
#define OS3_PAGETABLEENTRY_H

class PageTableEntry{

public:
    PageTableEntry(){
        entry[0] = 0;
        entry[1] = 0;
        entry[2] = 0;
        entry[3] = 0;
        entry[4] = -1;
    }

    bool isModified() { return entry[0] == 1; }
    bool isReferenced() { return entry[1] == 1; }
    bool isPresent() { return entry[2] == 1; }
    int getFrameNo() { return entry[3]; }
    int getDiskLoc() { return entry[4]; }

    void setModified() { entry[0] = 1; }
    void setUnModified() { entry[0] = 0; }
    void setReferenced() { entry[1] = 1; }
    void setUnReferenced() { entry[1] = 0; }
    void setPresent() { entry[2] = 1; }
    void setAbsent() { entry[2] = 0; }
    void setFrameNo(int n) { entry[3] = n; }
    void setDiskLoc(int n) { entry[4] = n; }

	
	/**
     *  0 -> Modified Bit
     *  1 -> Referenced Bit

     *  2 -> Present/Absent Bit
     *  3 -> Page Frame Number
     *  4 -> Actual disk location that page stored
     */
    int entry[5];

private:    

};

#endif //OS3_PAGETABLEENTRY_H
