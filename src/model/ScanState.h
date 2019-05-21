//
// Created by Florian Bruggisser on 2019-05-21.
//

#ifndef THREE_SCAN_SCANSTATE_H
#define THREE_SCAN_SCANSTATE_H


class ScanState {
public:
    volatile bool scanLoopRunning = true;
    volatile bool requestStartScan = false;
    volatile bool requestStopScan = false;
};


#endif //THREE_SCAN_SCANSTATE_H
